#ifndef BOUND_READ_ASSIGN_H_
#define BOUND_READ_ASSIGN_H_

#include "../read_status.h"
#include "../type_traits.h"

namespace bound
{

namespace read
{

template <typename A, typename B>
struct is_assignable
{
    const static bool value =
        std::is_same<A, B>::value ||
        std::is_assignable<A, B>::value;
};

template <typename A, typename B>
struct is_convertable
{
    const static bool value =
        !is_assignable<A, B>::value &&
        std::is_arithmetic<A>::value &&
        std::is_arithmetic<B>::value;
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
typename std::enable_if_t<is_assignable<A, B>::value>
Assign(A &a, B &b, ReadStatus &status)
{
    a = b;
}

// Conversion assign
template <typename A, typename B>
typename std::enable_if_t<is_convertable<A, B>::value>
Assign(A &a, B &b, ReadStatus &status)
{
    a = static_cast<A>(b);
}

// Incompatible assignment
template <typename A, typename B>
typename std::enable_if_t<is_unassignable<A, B>::value>
Assign(A &a, B &b, ReadStatus &status)
{
    std::string error_message = "Cannot assign type \"";
    error_message += typeid(A).name();
    error_message += "\" to type \"";
    error_message += typeid(B).name();
    error_message += "\".";
    status.set_error_message(error_message);
}

// Simple assign
template <typename A, typename B>
typename std::enable_if_t<is_assignable<A, B>::value>
Assign(A &a, B &&b, ReadStatus &status)
{
    a = b;
}

// Conversion assign
template <typename A, typename B>
typename std::enable_if_t<is_convertable<A, B>::value>
Assign(A &a, B &&b, ReadStatus &status)
{
    a = static_cast<A>(b);
}

// Incompatible assignment
template <typename A, typename B>
typename std::enable_if_t<is_unassignable<A, B>::value>
Assign(A &a, B &&b, ReadStatus &status)
{
    std::string error_message = "Cannot assign type \"";
    error_message += typeid(A).name();
    error_message += "\" to type \"";
    error_message += typeid(B).name();
    error_message += "\".";
    status.set_error_message(error_message);
}

} // namespace read

} // namespace bound

#endif