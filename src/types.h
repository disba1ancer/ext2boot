#ifndef TYPES_H
#define TYPES_H

#include <cstdint>

namespace e2bt {

using I8  = std::int8_t;
using I16 = std::int16_t;
using I32 = std::int32_t;
using I64 = std::int64_t;

using U8  = std::uint8_t;
using U16 = std::uint16_t;
using U32 = std::uint32_t;
using U64 = std::uint64_t;

using UChar = unsigned char;
using Byte = UChar;

struct UUID {
    U32 a;
    U16 b;
    U16 c;
    Byte e[8];
};

}

#endif // TYPES_H
