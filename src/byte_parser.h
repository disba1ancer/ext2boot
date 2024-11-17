#ifndef BYTE_PARSER_H
#define BYTE_PARSER_H

#include <span>
#include <stdexcept>
#include "types.h"

namespace e2bt {

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
        if constexpr (!pack) {
            auto align = std::ptrdiff_t(alignof(T));
            auto offset = alignOffset + next - seq.begin();
            skip((align - offset) & (align - 1));
        }
        if (seq.end() - next < sizeof(T)) {
            next = seq.end();
            throw std::runtime_error("End of byte sequence");
        }
        T result{};
        memcpy(std::addressof(result), seq.data() + (next - seq.begin()), sizeof(T));
        next += sizeof(T);
        return result;
    }
    template <typename T>
    auto& get(T& result)
    {
        if constexpr (!pack) {
            auto align = std::ptrdiff_t(alignof(T));
            auto offset = alignOffset + next - seq.begin();
            skip((align - offset) & (align - 1));
        }
        if (seq.end() - next < sizeof(T)) {
            next = seq.end();
            throw std::runtime_error("End of byte sequence");
        }
        memcpy(std::addressof(result), seq.data() + (next - seq.begin()), sizeof(T));
        next += sizeof(T);
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
