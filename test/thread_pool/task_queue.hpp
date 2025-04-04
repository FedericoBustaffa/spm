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
    task_queue(size_t capacity) : m_joined(false), m_capacity(capacity)
    {
    }

    inline size_t size() const
    {
        return m_tasks.size();
    }

    inline bool empty() const
    {
        return m_tasks.size() == 0;
    }

    inline bool full() const
    {
        return m_capacity == 0 ? false : m_tasks.size() >= m_capacity;
    }

    inline const size_t capacity() const
    {
        return m_capacity;
    }

    inline bool is_joined() const
    {
        return m_joined.load();
    }

    template <typename Func, typename... Args,
              typename Ret = typename std::result_of<Func(Args...)>::type>
    std::future<Ret> push(Func &&func, Args &&...args)
    {
        // push task into the queue
        std ::function<Ret(void)> aux_func =
            std::bind(std::forward<Func>(func), std::forward<Args>(args)...);

        auto promise = std::make_shared<std::promise<Ret>>();

        // make a void(void) function and store the result in a promise
        auto task = [promise, aux_func]() mutable {
            promise->set_value(aux_func());
        };

        if (m_joined.load())
            throw std::runtime_error("JOINED_QUEUE: can't submit more tasks");

        std::unique_lock<std::mutex> lock(m_mutex);
        while (this->full())
            m_full.wait(lock);

        m_tasks.push(task);

        m_empty.notify_one();
        lock.unlock();

        return promise->get_future();
    }

    template <typename Func, typename... Args,
              typename Ret = typename std::result_of<Func(Args...)>::type>
    std::future<Ret> push_async(Func &&func, Args &&...args)
    {
        if (this->full())
            throw std::runtime_error("FULL QUEUE: submission failed");

        if (m_joined.load())
            throw std::runtime_error("JOINED_QUEUE: can't submit more tasks");

        // push task into the queue
        std ::function<Ret(void)> aux_func =
            std::bind(std::forward<Func>(func), std::forward<Args>(args)...);

        auto promise = std::make_shared<std::promise<Ret>>();

        // make a void(void) function and store the result in a promise
        auto task = [promise, aux_func]() mutable {
            promise->set_value(aux_func());
        };

        std::unique_lock<std::mutex> lock(m_mutex);
        m_tasks.push(task);
        m_empty.notify_one();
        lock.unlock();

        return promise->get_future();
    }

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

    void join()
    {
        m_joined.store(true);
        m_empty.notify_all();
    }

    ~task_queue()
    {
        join();
    }

private:
    std::atomic<bool> m_joined;
    const size_t m_capacity;
    std::queue<std::function<void(void)>> m_tasks;

    std::mutex m_mutex;
    std::condition_variable m_empty, m_full;
};

#endif