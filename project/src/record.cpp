#include "record.hpp"

#include <cstring>

uint64_t record::s_counter = 0;

std::mt19937 record::s_generator = std::mt19937(42);

record::record() : m_key(0), m_length(0), m_payload(nullptr) {}

record::record(uint32_t length) : m_key(s_counter++), m_length(length)
{
    // fill the payload with random characters
    m_payload = new char[length];
    std::uniform_int_distribution<char> distribution(0, 127);
    for (uint32_t i = 0; i < length; i++)
        m_payload[i] = distribution(s_generator);
}

record::record(uint64_t key, uint32_t length, const char* payload)
    : m_key(key), m_length(length)
{
    m_payload = new char[length];
    std::memcpy(m_payload, payload, length);
}

record::record(const record& other)
    : m_key(other.m_key), m_length(other.m_length)
{
    m_payload = new char[m_length];
    std::memcpy(m_payload, other.m_payload, m_length);
}

record::record(record&& other) noexcept
    : m_key(other.m_key), m_length(other.m_length), m_payload(other.m_payload)
{
    // invalidate the record
    other.m_key = 0;
    other.m_length = 0;
    other.m_payload = nullptr;
}

void record::operator=(const record& other)
{
    m_key = other.m_key;
    m_length = other.m_length;

    delete[] m_payload;
    m_payload = new char[m_length];
    std::memcpy(m_payload, other.m_payload, m_length);
}

void record::operator=(record&& other) noexcept
{
    delete[] m_payload;

    m_key = other.m_key;
    m_length = other.m_length;
    m_payload = other.m_payload;

    // invalidate the record
    other.m_key = 0;
    other.m_length = 0;
    other.m_payload = nullptr;
}

bool record::operator==(const record& other) const
{
    if (m_key != other.m_key || m_length != other.m_length)
        return false;

    return std::memcmp(m_payload, other.m_payload, m_length) == 0;
}

bool record::operator!=(const record& other) const { return !(*this == other); }

record::~record() { delete[] m_payload; }
