#ifndef BYTE_PARSER_H
#define BYTE_PARSER_H

#include <bit>
#include <span>
#include <stdexcept>
#include "types.h"

namespace e2bt {

namespace byte_paser_detail {

template <typename T, typename It>
T direct_deserialize(It in)
{
    Byte val[sizeof(T)];
    for (std::size_t i = 0; i < sizeof(T); ++i, ++in) {
        val[i] = *in;
    }
    return std::bit_cast<T>(val);
}

template <typename T, typename It>
T reverse_deserialize(It in)
{
    Byte val[sizeof(T)];
    for (std::size_t i = 0; i < sizeof(T); ++i, ++in) {
        val[sizeof(T) - 1 - i] = *in;
    }
    return std::bit_cast<T>(val);
}

}

template <bool pack = false>
struct byte_parser
{
    byte_parser(std::span<Byte> seq, std::ptrdiff_t alignOffset = 0) :
        seq(seq),
        next(seq.begin()),
        alignOffset(alignOffset)
    {}
    template <typename T>
    auto get() -> T
    {
        using namespace byte_paser_detail;
        if constexpr (!pack) {
            auto align = std::ptrdiff_t(alignof(T));
            auto offset = alignOffset + next - seq.begin();
            skip((align - offset) & (align - 1));
        }
        if (seq.end() - next < sizeof(T)) {
            next = seq.end();
            throw std::runtime_error("End of byte sequence");
        }
        auto result = direct_deserialize<T>(next);
        next += sizeof(T);
        return result;
    }
    template <typename T>
    auto& get(T& result)
    {
        result = get<T>();
        return *this;
    }
    template <typename T, std::size_t N>
    auto& get(T(& result)[N])
    {
        for (std::size_t i = 0; i < N; ++i) {
            result[i] = get<T>();
        }
        return *this;
    }
    auto& skip(std::ptrdiff_t n)
    {
        if (seq.end() - next < n) {
            next = seq.end();
            throw std::runtime_error("End of byte sequence");
        }
        next += n;
        return *this;
    }
    void reset()
    {
        next = seq.begin();
    }
    using span = std::span<unsigned char>;
    span seq;
    span::iterator next;
    std::ptrdiff_t alignOffset;
};

} // namespace e2bt

#endif // BYTE_PARSER_H
