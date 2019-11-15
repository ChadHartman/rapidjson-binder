#ifndef BOUND_READ_ASSIGN_H_
#define BOUND_READ_ASSIGN_H_

#include "../type_traits.h"

// #define BOUND_READ_ASSIGN_H_DEBUG

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
        !is_convertable<A, B>::value &&
        !std::is_same<B, std::nullptr_t>::value;
};

// Null assign
template <typename A>
bool Assign(A &a, std::nullptr_t b)
{
#ifdef BOUND_READ_ASSIGN_H_DEBUG
    printf("Assign[std::nullptr_t]\n");
#endif
    a = A();
    return true;
}

// Simple assign
template <typename A, typename B>
typename std::enable_if<is_assignable<A, B>::value, bool>::type
Assign(A &a, B &b)
{
#ifdef BOUND_READ_ASSIGN_H_DEBUG
    printf("Assign[is_assignable]\n");
#endif
    a = b;
    return true;
}

// Conversion assign
template <typename A, typename B>
typename std::enable_if<is_convertable<A, B>::value, bool>::type
Assign(A &a, B &b)
{
#ifdef BOUND_READ_ASSIGN_H_DEBUG
    printf("Assign[is_convertable]\n");
#endif
    a = static_cast<A>(b);
    return true;
}

// Incompatible assignment
template <typename A, typename B>
typename std::enable_if<is_unassignable<A, B>::value, bool>::type
Assign(A &a, B &b)
{
    printf("B TYPE: %s\n", typeid(B).name());
#ifdef BOUND_READ_ASSIGN_H_DEBUG
    printf("Assign[is_unassignable]\n");
#endif
    return false;
}

} // namespace read

} // namespace bound

#endif