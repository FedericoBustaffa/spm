#ifndef RECORD_HPP
#define RECORD_HPP

#include <cstdint>
#include <random>

#define MAX_PAYLOAD 256

class record
{
public:
    record();

    record(uint64_t key, uint32_t length, char* payload);

    record(const record& other);

    record(record&& other) noexcept;

    inline const uint64_t& key() const { return m_key; }

    inline const uint32_t& length() const { return m_length; }

    inline const char* payload() const { return m_payload; }

    inline bool is_valid() const { return m_length > 0; }

    inline const char& operator[](uint32_t i) const { return m_payload[i]; }

    record& operator=(const record& other);

    record& operator=(record&& other) noexcept;

    bool operator==(const record& other) const;

    bool operator!=(const record& other) const;

    bool operator<(const record& other) const;

    ~record();

private:
    uint64_t m_key;
    uint32_t m_length;
    char* m_payload;

private: // static members for initialization
    static uint64_t s_counter;
    static std::mt19937 s_generator;
};

#endif
