#ifndef THREAD_POOL_HPP
#define THREAD_POOL_HPP

#include <functional>
#include <future>
#include <optional>
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

                func.value()(); // execute the task
            }
        };

        m_workers.reserve(n);
        for (size_t i = 0; i < n; i++)
            m_workers.emplace_back(work);
    }

    /**
     * @brief This class should not be copyable (maybe yes I'm not sure)
     *
     */
    thread_pool(const thread_pool& other) = delete;

    /**
     * @brief This class should not be movable (this time I'm pretty sure)
     *
     */
    thread_pool(thread_pool&& other) = delete;

    /**
     * @brief Returns false if the `shutdown` method is already been invoked,
     * true otherwise.
     *
     * @return bool
     */
    inline bool is_running() const { return m_running; }

    /**
     * @brief Returns the number of worker threads in the pool.
     *
     * @return size_t
     */
    inline size_t size() const { return m_workers.size(); }

    /**
     * @brief Returns the capacity of the task queue.
     *
     * @return size_t
     */
    inline size_t queue_capacity() const { return m_tasks.capacity(); }

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
    std::future<Ret> submit(Func&& func, Args&&... args)
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
     * @return std::future
     */
    template <typename Func, typename... Args,
              typename Ret = typename std::result_of<Func(Args...)>::type>
    std::future<Ret> submit_async(Func&& func, Args&&... args)
    {
        return m_tasks.push_async(std::forward<Func>(func),
                                  std::forward<Args>(args)...);
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
        m_tasks.close();
    }

    /**
     * @brief Calls shutdown method and wait for the threads to join.
     *
     */
    void join()
    {
        shutdown();
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
            this->join();
    }

private:
    bool m_running;
    std::vector<std::thread> m_workers;
    task_queue m_tasks;
};

#endif
