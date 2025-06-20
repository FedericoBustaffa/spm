#ifndef THREADPOOL_HPP
#define THREADPOOL_HPP

#include <functional>
#include <future>
#include <optional>
#include <thread>
#include <vector>

#include "lock_queue.hpp"

namespace spm
{

class threadpool
{
public:
    /**
     * @brief Construct a new thread pool object with `workers`
     * number of worker threads and a task queue with the given length.
     *
     * @param workers the number of thread workers. If not specified it will be
     * used the `std::thread::hardware_concurrency()` value.
     * @param capacity the size of the task queue. If not specified the
     * queue is unbounded.
     */
    threadpool(size_t workers = 0, size_t capacity = 0)
        : m_running(true), m_tasks(capacity)
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
    threadpool(const threadpool& other) = delete;

    /**
     * @brief This class should not be movable (this time I'm pretty sure)
     *
     */
    threadpool(threadpool&& other) = delete;

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
    inline size_t capacity() const { return m_tasks.capacity(); }

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
        // create the task
        std::packaged_task<Ret(void)> task = make_task(func, args...);

        // extract the future
        std::future<Ret> future = task.get_future();

        auto task_ptr = std::make_shared<decltype(task)>(std::move(task));

        std::function<void(void)> payload = [task_ptr]() -> void {
            task_ptr->operator()();
        };

        m_tasks.push(std::move(payload));

        return future;
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
    ~threadpool()
    {
        if (m_running)
            this->join();
    }

private:
    template <typename Func, typename... Args,
              typename Ret = typename std::result_of<Func(Args...)>::type>
    std::packaged_task<Ret(void)> make_task(Func&& func, Args&&... args)
    {
        std::function<Ret(void)> aux =
            std::bind(std::forward<Func>(func), std::forward<Args>(args)...);

        return std::packaged_task<Ret(void)>(aux);
    }

private:
    bool m_running;
    lock_queue<std::function<void(void)>> m_tasks;
    std::vector<std::thread> m_workers;
};

} // namespace spm

#endif
