#ifndef THREAD_POOL_HPP
#define THREAD_POOL_HPP

#include <vector>
#include <queue>
#include <functional>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>

class thread_pool
{
public:
	/**
	 * @brief Construct a new thread pool object with `workers`
	 * number of worker threads.
	 *
	 * @param workers
	 */
	thread_pool(size_t workers = 0) : m_running(true)
	{
		size_t n = workers == 0 ? std::thread::hardware_concurrency() : workers;

		// lambda executed by every worker
		auto work = [this]() {
			std::function<void(void)> func;
			while (true)
			{
				{
					std::unique_lock<std::mutex> lock(m_mutex);
					while (m_running && m_tasks.empty())
						m_empty.wait(lock);

					if (!m_running && m_tasks.empty())
						return;

					func = std::move(m_tasks.front());
					m_tasks.pop();
				}
				func();
			}
		};

		for (size_t i = 0; i < n; i++)
			m_workers.emplace_back(work);
	}

	/**
	 * @brief Returns the number of worker threads in the pool.
	 *
	 * @return Number of worker threads
	 */
	inline size_t size() const
	{
		return m_workers.size();
	}

	/**
	 * @brief Submits a task and wait for its completion.
	 *
	 * @param func the callable to execute
	 * @param args arguments for the callable
	 * @return The same result as the `func(args...)` call
	 */
	template <typename Func, typename... Args,
			  typename Ret = typename std::result_of<Func(Args...)>::type>
	Ret submit(Func&& func, Args&&... args)
	{
		std::function<Ret(void)> aux_func =
			std::bind(std::forward<Func>(func), std::forward<Args>(args)...);

		auto promise = std::make_shared<std::promise<Ret>>();

		// make a void(void) function and store the result in a promise
		auto task = [promise, aux_func]() mutable { promise->set_value(aux_func()); };

		// push task into the queue
		std::unique_lock<std::mutex> lock(m_mutex);
		m_tasks.push(task);
		m_empty.notify_one();
		lock.unlock();

		return promise->get_future().get();
	}

	/**
	 * @brief Submits a task a return a future to handle the result later.
	 *
	 * @param func the callable to execute
	 * @param args arguments for the callable
	 * @return An `std::future` that contains the same result as the
	 * `func(args...)` call
	 */
	template <typename Func, typename... Args,
			  typename Ret = typename std::result_of<Func(Args...)>::type>
	std::future<Ret> submit_async(Func&& func, Args&&... args)
	{
		std::function<Ret(void)> aux_func =
			std::bind(std::forward<Func>(func), std::forward<Args>(args)...);

		auto promise = std::make_shared<std::promise<Ret>>();

		// make a void(void) function and store the result in a promise
		auto task = [promise, aux_func]() mutable { promise->set_value(aux_func()); };

		// push task into the queue
		std::unique_lock<std::mutex> lock(m_mutex);
		m_tasks.push(task);
		m_empty.notify_one();

		return promise->get_future();
	}

	/**
	 * @brief Apply `func` to all elements of `v`. Every worker handle
	 * `chunksize` elements at a time.
	 *
	 * @param func the function to apply
	 * @param v the input vector
	 * @param chunksize the number of elements handled by a worker
	 * @return a new `std::vector<T>` containing the result
	 */
	template <typename Func, typename T>
	std::vector<T> map(Func&& func, const std::vector<T>& v, size_t chunksize = 1)
	{
		size_t chunks = (v.size() + chunksize - 1) / chunksize;
		std::vector<T> buffer(v.size());

		auto chunked_func = [&v, &buffer, &func](size_t start, size_t stop) mutable {
			stop = stop <= v.size() ? stop : v.size();
			for (size_t i = start; i < stop; i++)
				buffer[i] = func(v[i]);

			return 0;
		};

		std::vector<std::future<int>> futures;
		futures.reserve(chunks);
		for (size_t i = 0; i < chunks; i++)
			futures.push_back(submit_async(chunked_func, i * chunksize, i * chunksize + chunksize));

		for (auto& f : futures)
			f.get();

		return buffer;
	}

	/**
	 * @brief Apply `func` to all elements of `v` and returns a future
	 * containing the result vector. Every worker handle `chunksize` elements
	 * at a time.
	 *
	 * @param func the function to apply
	 * @param v the input vector
	 * @param chunksize the number of elements handled by a worker
	 * @return a `std::future` containing the result vector
	 */
	template <typename Func, typename T>
	std::future<std::vector<T>> map_async(Func&& func, const std::vector<T>& v,
										  size_t chunksize = 1)
	{
		size_t chunks = (v.size() + chunksize - 1) / chunksize;
		auto result = std::make_shared<std::vector<T>>(v.size());

		auto chunked_func = [&v, result, &func](size_t start, size_t stop) mutable {
			stop = stop <= v.size() ? stop : v.size();
			for (size_t i = start; i < stop; i++)
				(*result)[i] = func(v[i]);

			return 0;
		};

		auto futures = std::make_shared<std::vector<std::future<int>>>();
		futures->reserve(chunks);
		for (size_t i = 0; i < chunks; i++)
			futures->push_back(
				submit_async(chunked_func, i * chunksize, i * chunksize + chunksize));

		auto final_task = [result, futures]() mutable {
			for (auto& f : *futures)
				f.get();

			return std::move(*result);
		};

		return submit_async(final_task);
	}

	/**
	 * @brief Shuts down the thread pool. If there are some tasks pending, it
	 * blocks the execution until done. If a task is submitted after shutting
	 * down an exception is raised.
	 *
	 */
	void shutdown()
	{
		m_running = false;
		m_empty.notify_all();
		for (auto& w : m_workers)
			w.join();
	}

	/**
	 * @brief Destroy the thread pool object; if still running calls the
	 * `shutdown` method.
	 *
	 */
	~thread_pool()
	{
		if (m_running)
			this->shutdown();
	}

private:
	volatile bool m_running;
	std::vector<std::thread> m_workers;
	std::queue<std::function<void(void)>> m_tasks;
	std::mutex m_mutex;
	std::condition_variable m_empty;
};

#endif
