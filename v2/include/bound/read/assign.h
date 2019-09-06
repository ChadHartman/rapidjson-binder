#ifndef BOUND_READ_ASSIGN_H_
#define BOUND_READ_ASSIGN_H_

#include "../type_traits.h"

namespace bound
{

namespace read
{

template <typename V>
struct value_raw
{
    using type = typename std::remove_reference<
        typename std::remove_const<V>::type>::type;
};

template <typename A, typename B>
struct is_assignable
{
    const static bool value =
        std::is_same<A, typename value_raw<B>::type>::value ||
        std::is_assignable<A, typename value_raw<B>::type>::value;
};

template <typename A, typename B>
struct is_convertable
{
    const static bool value =
        !is_assignable<A, B>::value &&
        std::is_arithmetic<A>::value &&
        std::is_arithmetic<typename value_raw<B>::type>::value;
};

template <typename A, typename B>
struct is_unassignable
{
    const static bool value =
        !is_assignable<A, B>::value &&
        !is_convertable<A, B>::value;
};

// Simple assign
template <typename A, typename B>
typename std::enable_if<is_assignable<A, B>::value, bool>::type
Assign(A &a, B &b)
{
    a = b;
    return true;
}

// Conversion assign
template <typename A, typename B>
typename std::enable_if<is_convertable<A, B>::value, bool>::type
Assign(A &a, B &b)
{
    a = static_cast<A>(b);
    return true;
}

// Incompatible assignment
template <typename A, typename B>
typename std::enable_if<is_unassignable<A, B>::value, bool>::type
Assign(A &a, B &b)
{
    return false;
}

} // namespace read

} // namespace bound

#endif