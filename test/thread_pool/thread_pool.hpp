#ifndef THREAD_POOL_HPP
#define THREAD_POOL_HPP

#include <condition_variable>
#include <functional>
#include <future>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

class thread_pool
{
public:
    /**
     * @brief Construct a new thread pool object with `workers`
     * number of worker threads and a task queue with the given length.
     *
     * @param workers the number of thread workers. If not specified it will be
     * used the `std::thread::hardware_concurrency()` value.
     * @param queue_capacity the size of the task queue. If not specified the
     * queue is unbounded.
     */
    thread_pool(size_t workers = 0, size_t queue_capacity = 0)
        : m_running(true), m_queue_capacity(queue_capacity)
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
     * @return size_t
     */
    inline size_t size() const
    {
        return m_workers.size();
    }

    /**
     * @brief Returns the capacity of the task queue.
     *
     * @return size_t
     */
    inline size_t queue_capacity() const
    {
        return m_queue_capacity;
    }

    /**
     * @brief Submits a task and returns a future to handle the result. If the
     * queue is full it blocks until the job is submitted.
     *
     * @param func the callable to execute
     * @param args arguments for the callable
     * @return a future to handle the result
     */
    template <typename Func, typename... Args,
              typename Ret = typename std::result_of<Func(Args...)>::type>
    std::future<Ret> submit(Func &&func, Args &&...args)
    {
        auto [task, future] =
            make_task(std::forward<Func>(func), std::forward<Args>(args)...);

        push(task);

        return future;

        return promise->get_future().get();
    }

    /**
     * @brief Submits a task a return a future to handle the result later.
     *
     * @param func the callable to execute
     * @param args arguments for the callable
     * @return `std::future`
     */
    template <typename Func, typename... Args,
              typename Ret = typename std::result_of<Func(Args...)>::type>
    std::future<Ret> submit_async(Func &&func, Args &&...args)
    {
        auto task =
            make_task(std::forward<Func>(func), std::forward<Args>(args)...);

        return push_async(task);
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
    std::vector<T> map(Func &&func, const std::vector<T> &v,
                       size_t chunksize = 1)
    {
        size_t chunks = (v.size() + chunksize - 1) / chunksize;

        std::vector<T> buffer(v.size());

        auto chunked_func = [&v, &buffer, &func](size_t start,
                                                 size_t stop) mutable {
            stop = stop <= v.size() ? stop : v.size();
            for (size_t i = start; i < stop; i++)
                buffer[i] = func(v[i]);

            return 0;
        };

        std::vector<std::future<int>> futures;
        futures.reserve(chunks);
        for (size_t i = 0; i < chunks; i++)
            futures.push_back(submit_async(chunked_func, i * chunksize,
                                           i * chunksize + chunksize));

        for (auto &f : futures)
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
    std::future<std::vector<T>> map_async(Func &&func, const std::vector<T> &v,
                                          size_t chunksize = 1)
    {
        size_t chunks = (v.size() + chunksize - 1) / chunksize;
        auto result = std::make_shared<std::vector<T>>(v.size());

        auto chunked_func = [&v, result, &func](size_t start,
                                                size_t stop) mutable {
            stop = stop <= v.size() ? stop : v.size();
            for (size_t i = start; i < stop; i++)
                (*result)[i] = func(v[i]);

            return 0;
        };

        auto futures = std::make_shared<std::vector<std::future<int>>>();
        futures->reserve(chunks);
        for (size_t i = 0; i < chunks; i++)
            futures->push_back(submit_async(chunked_func, i * chunksize,
                                            i * chunksize + chunksize));

        auto final_task = [result, futures]() mutable {
            for (auto &f : *futures)
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
        for (auto &w : m_workers)
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
    template <typename Func, typename... Args,
              typename Ret = typename std::result_of<Func(Args...)>>
    std::future<Ret> push(Func &&func, Args &&...args)
    {
        // push task into the queue
        std::function<Ret(void)> aux_func =
            std::bind(std::forward<Func>(func), std::forward<Args>(args)...);

        auto promise = std::make_shared<std::promise<Ret>>();

        // make a void(void) function and store the result in a promise
        auto task = [promise, aux_func]() mutable {
            promise->set_value(aux_func());
        };

        std::unique_lock<std::mutex> lock(m_mutex);
        while (m_tasks.size() == m_queue_capacity)
            m_full.wait(lock);

        m_tasks.push(task);

        m_empty.notify_one();
        lock.unlock();

        return promise->get_future();
    }

private:
    bool m_running;
    std::vector<std::thread> m_workers;

    // queue
    size_t m_queue_capacity;
    std::queue<std::function<void(void)>> m_tasks;
    std::mutex m_mutex;
    std::condition_variable m_empty;
};

#endif
