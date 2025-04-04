#ifndef THREAD_POOL_HPP
#define THREAD_POOL_HPP

#include <condition_variable>
#include <functional>
#include <future>
#include <mutex>
#include <optional>
#include <queue>
#include <thread>
#include <vector>

#include "task_queue.hpp"
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
        : m_running(true), m_tasks(queue_capacity)
    {
        size_t n = workers == 0 ? std::thread::hardware_concurrency() : workers;

        // lambda executed by every worker
        auto work = [this]() {
            std::optional<std::function<void(void)>> func;
            while (true)
            {
                func = m_tasks.pop();
                if (!func.has_value())
                    return;

                func.value()();
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
        return m_tasks.capacity();
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
        return m_tasks.push(std::forward<Func>(func),
                            std::forward<Args>(args)...);
    }

    /**
     * @brief Submits a task a return a future to handle the result later. If
     * the task queue is full throws a `std::runtime_exception`
     *
     * @param func the callable to execute
     * @param args arguments for the callable
     * @return `std::future`
     */
    template <typename Func, typename... Args,
              typename Ret = typename std::result_of<Func(Args...)>::type>
    std::future<Ret> submit_async(Func &&func, Args &&...args)
    {
        return m_tasks.push_async(std::forward<Func>(func),
                                  std::forward<Args>(args)...);
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

        // compute the function on the given chunk
        auto chunked_func = [&v, &func](std::vector<T> *result, size_t chunk,
                                        size_t chunksize) mutable {
            size_t start = chunk * chunksize;
            size_t stop = chunk * chunksize + chunksize;
            stop = stop <= v.size() ? stop : v.size();
            for (size_t i = start; i < stop; i++)
                (*result)[i] = func(v[i]);

            return 0;
        };

        // task that submits chunk tasks
        auto submission = [this, &v, &chunked_func, &chunks,
                           &chunksize]() mutable {
            std::vector<T> result(v.size());
            std::vector<std::future<int>> futures;
            futures.reserve(chunks);
            for (size_t i = 0; i < chunks; i++)
                futures.push_back(submit(chunked_func, &result, i, chunksize));

            for (auto &f : futures)
                f.get();

            return result;
        };

        return submit(submission);
    }

    /**
     * @brief Apply `func` to all elements of `v` and blocks until done.
     * Every worker handle `chunksize` elements at a time.
     *
     * @param func the function to apply. The only argument must be one
     * element of the vector.
     * @param v the input vector
     * @param chunksize the number of elements handled by a worker
     * @return a new `std::vector<T>` containing the result
     */
    template <typename Func, typename T>
    std::vector<T> map(Func &&func, const std::vector<T> &v,
                       size_t chunksize = 1)
    {
        auto future = map_async(std::forward<Func>(func), v, chunksize);
        return future.get();
    }

    /**
     * @brief Shuts down the thread pool. If there are some tasks pending,
     * it blocks the execution until done. If a task is submitted after
     * shutting down an exception is raised.
     *
     */
    void shutdown()
    {
        m_running = false;
        m_tasks.join();
    }

    /**
     * @brief Calls shutdown method and wait for the threads to join
     *
     */
    void join()
    {
        shutdown();
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
            this->join();
    }

private:
    bool m_running;
    std::vector<std::thread> m_workers;
    task_queue m_tasks;
};

#endif
