#ifndef TASK_QUEUE_HPP
#define TASK_QUEUE_HPP

#include <atomic>
#include <condition_variable>
#include <functional>
#include <future>
#include <memory>
#include <mutex>
#include <optional>
#include <queue>

class task_queue
{
public:
    /**
     * @brief Construct a new task queue object with the given capacity. If
     * capacity is equal to 0 the queue is unbounded.
     *
     * @param capacity the capacity of the queue.
     */
    task_queue(size_t capacity) : m_joined(false), m_capacity(capacity) {}

    /**
     * @brief Returns the number of tasks in the queue.
     *
     */
    inline size_t size() const
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_tasks.size();
    }

    /**
     * @brief Check if the queue is empty or not.
     *
     * @return true if the size of the queue is equal to 0.
     * @return false otherwise
     */
    inline bool empty() const
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_tasks.size() == 0;
    }

    /**
     * @brief Check if the queue is full or not.
     *
     * @return true if the size of the queue is equal to its capacity.
     * @return false otherwise
     */
    inline bool full() const
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_capacity == 0 ? false : m_tasks.size() >= m_capacity;
    }

    /**
     * @brief Returns the capacity of the queue.
     *
     */
    inline size_t capacity() const { return m_capacity; }

    /**
     * @brief Push a task into the queue and returns a future to handle the
     * result. If the queue is full it blocks until the task is enqueued.
     *
     * @param func the function to execute.
     * @param args its arguments.
     * @return a future with the return value of the passed function.
     */
    template <typename Func, typename... Args,
              typename Ret = typename std::invoke_result<Func, Args...>::type>
    std::future<Ret> push(Func&& func, Args&&... args)
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        if (m_joined.load())
            throw std::runtime_error("JOINED_QUEUE: can't submit more tasks");

        while (m_capacity == 0 ? false : m_tasks.size() >= m_capacity)
            m_full.wait(lock);

        return make_task(std::forward<Func>(func), std::forward<Args>(args)...);
    }

    /**
     * @brief Push a task into the queue and returns a future to handle the
     * result. If the queue is full it throw an exception.
     *
     * @param func the function to execute.
     * @param args its arguments.
     * @return a future with the return value of the passed function.
     */
    template <typename Func, typename... Args,
              typename Ret = typename std::invoke_result<Func, Args...>::type>
    std::future<Ret> push_async(Func&& func, Args&&... args)
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        if (m_joined.load())
            throw std::runtime_error("JOINED_QUEUE: can't submit more tasks");

        if (m_capacity == 0 ? false : m_tasks.size() >= m_capacity)
            throw std::runtime_error("FULL_QUEUE: submission failed");

        return make_task(std::forward<Func>(func), std::forward<Args>(args)...);
    }

    /**
     * @brief Extract a task from the queue. If the queue is empty it blocks
     * until a new task is consumed.
     *
     * @return A void(void) function. It can be return nothing if the queue is
     * joined and tasks are in the queue.
     */
    std::optional<std::function<void(void)>> pop()
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        while (m_tasks.empty() && !m_joined.load())
            m_empty.wait(lock);

        if (m_tasks.empty())
            return std::nullopt;

        std::function<void(void)> task = std::move(m_tasks.front());
        m_tasks.pop();

        m_full.notify_one();

        return task;
    }

    /**
     * @brief Check if the queue is joined or not.
     *
     * @return true if the queue has already joined.
     * @return false otherwise.
     */
    inline bool is_joined() const { return m_joined.load(); }

    /**
     * @brief Join the queue and notifies all the other threads waiting on
     * `pop`.
     *
     */
    void join()
    {
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_joined.store(true);
        }
        m_empty.notify_all();
    }

    /**
     * @brief Destroy the task queue object and calls `join` method.
     *
     */
    ~task_queue() { join(); }

private:
    template <typename Func, typename... Args,
              typename Ret = typename std::invoke_result<Func, Args...>::type>
    std::future<Ret> make_task(Func&& func, Args&&... args)
    {
        // push task into the queue
        std ::function<Ret(void)> aux_func =
            std::bind(std::forward<Func>(func), std::forward<Args>(args)...);

        auto promise = std::make_shared<std::promise<Ret>>();

        // make a void(void) function and store the result in a promise
        auto task = [promise, aux_func]() mutable {
            try
            {
                promise->set_value(aux_func());
            }
            catch (...)
            {
                promise->set_exception(std::current_exception());
            }
        };

        m_tasks.push(task);
        m_empty.notify_one();

        return promise->get_future();
    }

private:
    std::atomic<bool> m_joined;
    const size_t m_capacity;
    std::queue<std::function<void(void)>> m_tasks;

    mutable std::mutex m_mutex;
    std::condition_variable m_empty, m_full;
};

#endif
