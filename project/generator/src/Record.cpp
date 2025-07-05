#include "Record.hpp"

#include <cstring>

uint64_t Record::s_Counter = 0;

std::mt19937 Record::s_Generator = std::mt19937(std::random_device()());

Record::Record(uint32_t length) : m_Key(s_Counter++), m_Length(length)
{
    m_Payload = new uint8_t[length];
    std::uniform_int_distribution<uint8_t> distribution(0, 255);
    for (uint32_t i = 0; i < length; i++)
        m_Payload[i] = distribution(s_Generator);
}

Record::Record(uint64_t key, uint32_t length, const uint8_t* payload)
    : m_Key(key), m_Length(length)
{
    m_Payload = new uint8_t[length];
    std::memcpy(m_Payload, payload, length);
}

Record::Record(const Record& other)
    : m_Key(other.m_Key), m_Length(other.m_Length)
{
    m_Payload = new uint8_t[m_Length];
    std::memcpy(m_Payload, other.m_Payload, m_Length);
}

Record::Record(Record&& other) : m_Key(other.m_Key), m_Length(other.m_Length)
{
    other.m_Key = 0;
    other.m_Length = 0;

    m_Payload = other.m_Payload;
    m_Payload = nullptr;
}

Record::~Record() { delete[] m_Payload; }
