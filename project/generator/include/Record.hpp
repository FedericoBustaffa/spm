#ifndef RECORD_HPP
#define RECORD_HPP

#include <cstdint>
#include <random>

#define MAX_PAYLOAD 1024

class Record
{
public:
    // --- CONSTRUCTORS ---
    Record(uint32_t length);

    Record(uint64_t key, uint32_t length, const char* payload);

    Record(const Record& other);

    Record(Record&& other) noexcept;

    // --- GETTERS ---
    inline uint64_t key() const { return m_Key; }

    inline uint32_t length() const { return m_Length; }

    inline const char* payload() const { return m_Payload; }

    inline bool isValid() const { return m_Length > 0; }

    inline const char& operator[](uint32_t i) const { return m_Payload[i]; }

    // --- OPERATOR ---
    void operator=(const Record& other);

    void operator=(Record&& other) noexcept;

    bool operator==(const Record& other) const;

    bool operator!=(const Record& other) const;

    // --- DESTRUCTOR ---
    ~Record();

private: // static members for initialization
    static uint64_t s_Counter;
    static std::mt19937 s_Generator;

private:
    uint64_t m_Key;
    uint32_t m_Length;
    char* m_Payload;
};

#endif
