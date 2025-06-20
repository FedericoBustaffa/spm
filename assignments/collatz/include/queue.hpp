#ifndef QUEUE_HPP
#define QUEUE_HPP

#include <cstddef>
#include <cstdint>
#include <utility>

class queue
{
public:
    queue(size_t capacity);

    inline size_t capacity() const { return m_Capacity; }

    inline size_t size() const { return m_Size; }

    virtual void push(std::pair<size_t, uint64_t>);

    virtual std::pair<size_t, uint64_t> pop();

    virtual ~queue();

protected:
    size_t m_Capacity;
    size_t m_Size;
    std::pair<size_t, uint64_t>* m_Data;
};

class lock_queue : public queue
{
public:
    lock_queue(size_t capacity);

    void push(std::pair<size_t, uint64_t>);

    std::pair<size_t, uint64_t> pop();

    ~lock_queue();
};

class atomic_queue : public queue
{
public:
    atomic_queue(size_t capacity);

    void push(std::pair<size_t, uint64_t>);

    std::pair<size_t, uint64_t> pop();

    ~atomic_queue();
};

#endif
