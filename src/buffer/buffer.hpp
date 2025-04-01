#ifndef BUFFER_HPP
#define BUFFER_HPP

#include <mutex>
#include <condition_variable>

template <typename T>
class buffer
{
public:
	buffer(size_t capacity) : m_size(0), m_capacity(capacity) { m_buffer = new T[capacity]; }

	inline size_t size() const { return m_size; }

	inline size_t capacity() const { return m_capacity; }

	void push(const T& element)
	{
		std::unique_lock<std::mutex> lock(m_mtx);
		while (m_size >= m_capacity)
			m_full.wait(lock);

		if (m_size < m_capacity)
			m_buffer[m_size++] = element;

		m_empty.notify_one();
	}

	T pop()
	{
		std::unique_lock<std::mutex> lock(m_mtx);
		while (m_size == 0)
			m_empty.wait(lock);

		T element = m_buffer[m_size--];

		m_full.notify_one();

		return element;
	}

	~buffer() { delete[] m_buffer; }

private:
	size_t m_size;
	size_t m_capacity;
	T* m_buffer;

	// thread-safety
	std::mutex m_mtx;
	std::condition_variable m_full, m_empty;
};

#endif
