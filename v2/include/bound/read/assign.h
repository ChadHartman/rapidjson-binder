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

template <typename T, typename M, typename V>
typename std::enable_if_t<std::is_member_object_pointer<M>::value>
Set(T &instance, M property, V &value, ReadStatus &status)
{
    Assign(instance.*(property), value, status);
}

template <typename T, typename M, typename V>
typename std::enable_if_t<is_setter<M>::value>
Set(T &instance, M property, V &value, ReadStatus &status)
{
}

template <typename T, typename M, typename V>
typename std::enable_if_t<
    !is_setter<M>::value &&
    !std::is_member_object_pointer<M>::value>
Set(T &instance, M property, V &value, ReadStatus &status)
{
}

template <typename T, typename V>
void SetValue(T &instance, std::string &key, V &value, ReadStatus &read_status)
{
    bool found = false;

    ListProperties(instance, [&](auto &property) {
        if (key != property.name)
        {
            return;
        }

        found = true;

        Set(instance, property.member, value, read_status);
    });
}

template <typename T, typename V>
void SetValue(T &instance, std::string &key, V &&value, ReadStatus &read_status)
{
    SetValue(instance, key, value, read_status);
}

} // namespace read

} // namespace bound

#endif