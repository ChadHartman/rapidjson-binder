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

#ifndef BOUND_UTIL_H_
#define BOUND_UTIL_H_

#include <type_traits>
#include <tuple>
#include <utility>
#include <vector>
#include <list>
#include <deque>
#include <string>

namespace bound
{

namespace util
{

// Executes a lambda per integer contained in the integer_sequence
// Example:
//      int count = 4;
//      for_sequence(std::make_index_sequence<count>{}, [&](auto i) {
//          //...
//      }
// Adapted from Guillaume Racicot and fiorentinoing's StackOverflow answers:
// https://stackoverflow.com/questions/34090638/c-convert-json-to-object
template <typename T, T... S, typename F>
constexpr void for_sequence(std::integer_sequence<T, S...>, F &&f)
{
    using unpack_t = int[];
    (void)unpack_t{(static_cast<void>(f(std::integral_constant<T, S>{})), 0)..., 0};
}

// Adapted from Nawaz's answer on Stack Overflow:
//  https://stackoverflow.com/questions/9065081/how-do-i-get-the-argument-types-of-a-function-pointer-in-a-variadic-template-cla#9065203
template <typename T>
struct function_traits;

template <typename R, class Class, typename... Args>
struct function_traits<R (Class::*)(Args...)>
{
    constexpr static const std::size_t arg_count = sizeof...(Args);
    using return_type = R;

    template <size_t i>
    struct arg
    {
        typedef typename std::tuple_element_t<i, std::tuple<Args...>> type;
    };
};

// Strips out const, reference, and pointer
template <typename T>
struct raw_type
{
    using type = typename std::remove_const<
        typename std::remove_reference<
            typename std::remove_pointer<
                T>::type>::type>::type;
};

// Detects whether anything is a pointer to a class method similar to
// the `void method_name(T value)` signature
template <typename T>
struct is_setter : std::false_type
{
};

template <class Class, typename V>
struct is_setter<void (Class::*)(V)>
{
    enum
    {
        value = !std::is_same<void, V>::value
    };
};

// Detects whether anything is a pointer to a class method similar to
// the `T method_name()` signature
template <typename T>
struct is_getter : std::false_type
{
};

template <typename R, class Class>
struct is_getter<R (Class::*)()>
{
    enum
    {
        value = !std::is_same<void, R>::value
    };
};

// Detects whether anything is a pointer to a class method similar to
// the `T method_name()` signature
template <typename T>
struct is_authorizer : std::false_type
{
};

template <class Class>
struct is_authorizer<bool (Class::*)(const char*)> : std::true_type
{
};

// Detects whether anything is a deque, list, or vector
template <typename T>
struct is_seq_container : std::false_type
{
};

template <template <typename...> class Container, typename T>
struct is_seq_container<Container<T>>
{
    enum
    {
        value = std::is_same<Container<T>, std::vector<T>>::value ||
                std::is_same<Container<T>, std::deque<T>>::value ||
                std::is_same<Container<T>, std::list<T>>::value
    };
};

template <typename T>
struct is_integer
{
    enum
    {
        // bools are considered "unsigned", so bool checkes are omitted
        value = std::is_signed<T>::value && std::is_integral<T>::value
    };
};

template <typename T>
struct is_unsigned_integer
{
    enum
    {
        // floats are considered "signed", so integral check is omitted
        value = std::is_unsigned<T>::value && !std::is_same<T, bool>::value
    };
};

// Returns true if T is
//  int, uint, float, bool, etc
//  std::string
template <typename T>
struct is_simple_property
{
    enum
    {
        value = std::is_arithmetic<typename raw_type<T>::type>::value ||
                std::is_same<std::string, typename raw_type<T>::type>::value
    };
};

} // namespace util

} // namespace bound

#endif