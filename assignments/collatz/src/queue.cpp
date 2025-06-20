#include "queue.hpp"

// --- QUEUE ---
queue::queue(size_t capacity) : m_Capacity(capacity), m_Size(0)
{
    m_Data = new std::pair<size_t, uint64_t>[capacity];
}

void queue::push(std::pair<size_t, uint64_t> idx_value) {}

std::pair<size_t, uint64_t> queue::pop() { return {}; }

queue::~queue() { delete[] m_Data; }

// --- LOCK QUEUE ---

// --- ATOMIC QUEUE ---
