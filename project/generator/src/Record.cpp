#include "Record.hpp"

#include <cstring>

uint64_t Record::s_Counter = 0;

std::mt19937 Record::s_Generator = std::mt19937(42);

Record::Record(uint32_t length) : m_Key(s_Counter++), m_Length(length)
{
    // fill the payload with random characters
    m_Payload = new char[length];
    std::uniform_int_distribution<char> distribution(0, 127);
    for (uint32_t i = 0; i < length; i++)
        m_Payload[i] = distribution(s_Generator);
}

Record::Record(uint64_t key, uint32_t length, const char* payload)
    : m_Key(key), m_Length(length)
{
    m_Payload = new char[length];
    std::memcpy(m_Payload, payload, length);
}

Record::Record(const Record& other)
    : m_Key(other.m_Key), m_Length(other.m_Length)
{
    m_Payload = new char[m_Length];
    std::memcpy(m_Payload, other.m_Payload, m_Length);
}

Record::Record(Record&& other) noexcept
    : m_Key(other.m_Key), m_Length(other.m_Length), m_Payload(other.m_Payload)
{
    // invalidate the record
    other.m_Key = 0;
    other.m_Length = 0;
    other.m_Payload = nullptr;
}

void Record::operator=(const Record& other)
{
    m_Key = other.m_Key;
    m_Length = other.m_Length;

    delete[] m_Payload;
    m_Payload = new char[m_Length];
    std::memcpy(m_Payload, other.m_Payload, m_Length);
}

void Record::operator=(Record&& other) noexcept
{
    delete[] m_Payload;

    m_Key = other.m_Key;
    m_Length = other.m_Length;
    m_Payload = other.m_Payload;

    // invalidate the record
    other.m_Key = 0;
    other.m_Length = 0;
    other.m_Payload = nullptr;
}

bool Record::operator==(const Record& other) const
{
    if (m_Key != other.m_Key || m_Length != other.m_Length)
        return false;

    for (uint32_t i = 0; i < m_Length; i++)
        if (m_Payload[i] != other.m_Payload[i])
            return false;

    return true;
}

bool Record::operator!=(const Record& other) const { return !(*this == other); }

Record::~Record() { delete[] m_Payload; }
