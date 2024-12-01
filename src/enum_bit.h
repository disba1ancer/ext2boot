#ifndef ENUM_BIT_H
#define ENUM_BIT_H

#include <type_traits>

namespace e2bt {

template <typename T>
requires std::is_enum_v<T>
struct enum_bit : std::false_type {};

template <typename T>
constexpr auto enum_bit_v = enum_bit<T>::value;

namespace enum_bit_detail {

template <typename T>
concept Ena = enum_bit_v<T>;

template <typename T>
concept Integral = std::is_integral_v<T>;

template <Ena T>
auto underlying(T t) -> std::underlying_type_t<T>
{
    return static_cast<std::underlying_type_t<T>>(t);
}

} // namespace enum_bit_detail

namespace enum_bit_ops {

template <enum_bit_detail::Ena T>
T operator ~(const T& a)
{
    return {~enum_bit_detail::underlying(a)};
}

#define GENERATE(op) template <enum_bit_detail::Ena T>\
T operator op(const T& a, const T& b)\
{\
    return static_cast<T>(enum_bit_detail::underlying(a) op enum_bit_detail::underlying(b));\
}\
template <enum_bit_detail::Ena T>\
T& operator op##=(T& a, const T& b)\
{\
    a = a op b;\
    return a;\
}

GENERATE(|)
GENERATE(&)
GENERATE(^)

#undef GENERATE

// #define GENERATE(op) template <enum_bit_detail::Ena T, enum_bit_detail::Integral V>\
// T operator op(const T& a, const V& b)\
// {\
//     return static_cast<T>(enum_bit_detail::underlying(a) op enum_bit_detail::underlying(b));\
// }\
// template <enum_bit_detail::Ena T, enum_bit_detail::Integral V>\
// T& operator op##=(T& a, const V& b)\
// {\
//     a = a op b;\
//     return a;\
// }

// GENERATE(<<)
// GENERATE(>>)

// #undef GENERATE

} // namespace enum_bit_ops
} // namespace e2bt

#endif // ENUM_BIT_H
