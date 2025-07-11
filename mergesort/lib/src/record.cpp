#include "record.hpp"

#include <cstring>

record::record() : m_key(0), m_length(0), m_payload(nullptr) {}

record::record(uint64_t key, uint32_t length, char* payload)
    : m_key(key), m_length(length), m_payload(payload)
{
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

record& record::operator=(const record& other)
{
    m_key = other.m_key;
    m_length = other.m_length;

    delete[] m_payload;
    m_payload = new char[m_length];
    std::memcpy(m_payload, other.m_payload, m_length);

    return *this;
}

record& record::operator=(record&& other) noexcept
{
    delete[] m_payload;

    m_key = other.m_key;
    m_length = other.m_length;
    m_payload = other.m_payload;

    // invalidate the record
    other.m_key = 0;
    other.m_length = 0;
    other.m_payload = nullptr;

    return *this;
}

bool record::operator==(const record& other) const
{
    if (m_key != other.m_key || m_length != other.m_length)
        return false;

    return std::memcmp(m_payload, other.m_payload, m_length) == 0;
}

bool record::operator!=(const record& other) const { return !(*this == other); }

bool record::operator<(const record& other) const
{
    return m_key < other.m_key;
}

record::~record() { delete[] m_payload; }
