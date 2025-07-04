#ifndef RECORD_HPP
#define RECORD_HPP

#include <cstdint>
#include <random>

class Record
{
public:
    Record(uint32_t length);

    Record(const Record& other);

    Record(Record&& other);

    inline uint64_t key() const { return m_Key; }

    inline uint32_t length() const { return m_Length; }

    inline const uint8_t* payload() const { return m_Payload; }

    inline const uint8_t& operator[](uint32_t i) const { return m_Payload[i]; }

    inline bool isValid() const { return m_Length > 0; }

    ~Record();

private: // static members for initialization
    static uint64_t s_Counter;
    static std::mt19937 s_Generator;

private:
    uint64_t m_Key;
    uint32_t m_Length;
    uint8_t* m_Payload;
};

#endif
