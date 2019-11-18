/*
Copyright 2019 Chad Hartman

Permission is hereby granted, free of charge, to any person obtaining a copy of 
this software and associated documentation files (the "Software"), to deal in the 
Software without restriction, including without limitation the rights to use, copy, 
modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, 
and to permit persons to whom the Software is furnished to do so, subject to the 
following conditions:

The above copyright notice and this permission notice shall be included in all 
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, 
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT 
HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION 
OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE 
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. 
*/

#ifndef BOUND_READ_ASSIGN_H_
#define BOUND_READ_ASSIGN_H_

#include "../type_traits.h"

#define BOUND_READ_ASSIGN_H_DEBUG

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
#ifdef BOUND_READ_ASSIGN_H_DEBUG
    printf("Assign[is_unassignable]\n");
#endif
    return false;
}

} // namespace read

} // namespace bound

#endif