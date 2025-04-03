#ifndef TASK_QUEUE_HPP
#define TASK_QUEUE_HPP

#include <condition_variable>
#include <functional>
#include <future>
#include <memory>
#include <mutex>
#include <queue>

class task_queue
{
public:
    task_queue(size_t capacity) : m_capacity(capacity), m_joined(false)
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

    inline size_t capacity() const
    {
        return m_capacity;
    }

    inline bool is_joined() const
    {
        return m_joined;
    }

    template <typename Func, typename... Args,
              typename Ret = typename std::result_of<Func(Args...)>::type>
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
        while (m_tasks.size() == m_capacity)
            m_full.wait(lock);

        m_tasks.push(task);

        m_empty.notify_one();
        lock.unlock();

        return promise->get_future();
    }

    // template <typename Func, typename... Args,
    //           typename Ret = typename std::result_of<Func(Args...)>::type>
    // std::future<Ret> push_async(Func &&func, Args &&...args)
    // {
    //     // push task into the queue
    //     std::function<Ret(void)> aux_func =
    //         std::bind(std::forward<Func>(func), std::forward<Args>(args)...);

    //     auto promise = std::make_shared<std::promise<Ret>>();

    //     // make a void(void) function and store the result in a promise
    //     auto task = [promise, aux_func]() mutable {
    //         promise->set_value(aux_func());
    //     };

    //     std::unique_lock<std::mutex> lock(m_mutex);
    //     while (m_tasks.size() == m_capacity)
    //         m_full.wait(lock);

    //     m_tasks.push(task);

    //     m_empty.notify_one();
    //     lock.unlock();

    //     return promise->get_future();
    // }

    std::function<void(void)> pop()
    {
        return []() {};
    }

    void join()
    {
        m_joined = true;
    }

    ~task_queue()
    {
        join();
    }

private:
    volatile bool m_joined;
    size_t m_capacity;
    std::queue<std::function<void(void)>> m_tasks;

    std::mutex m_mutex;
    std::condition_variable m_empty, m_full;
};

#endif