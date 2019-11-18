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

#ifndef BOUND_TYPE_TRAITS_H_
#define BOUND_TYPE_TRAITS_H_

#include <type_traits>
#include <vector>
#include <list>
#include <deque>

namespace bound
{

template <typename T>
class is_bound
{
    typedef char does;
    typedef long doesnt;

    template <typename C>
    static does test(decltype(&C::BOUND_PROPS_NAME));

    template <typename C>
    static doesnt test(...);

public:
    constexpr static bool value = sizeof(test<T>(0)) == sizeof(char);
};

// Detects whether anything is a deque, list, or vector
template <typename T>
struct is_seq_container : std::false_type
{
};

template <template <typename...> class Container, typename T>
struct is_seq_container<Container<T>>
{
    constexpr static bool value =
        std::is_same<Container<T>, std::vector<T>>::value ||
        std::is_same<Container<T>, std::deque<T>>::value ||
        std::is_same<Container<T>, std::list<T>>::value;
};

template <typename T>
struct is_json_properties : std::false_type
{
};

template <typename V>
struct is_json_properties<std::map<std::string, V>> : std::true_type
{
};

template <typename T>
struct is_int
{
    constexpr static bool value =
        std::is_integral<T>::value &&
        std::is_signed<T>::value;
};

template <typename T>
struct is_uint
{
    constexpr static bool value =
        std::is_integral<T>::value &&
        std::is_unsigned<T>::value &&
        !std::is_same<T, bool>::value;
};

template <typename T>
struct is_getter : std::false_type
{
};

template <typename T, typename Class>
struct is_getter<T (Class::*)()>
{
    const static bool value = !std::is_same<void, T>::value;
};

template <typename T, typename Class>
struct is_getter<T (Class::*)() const> : std::true_type
{
};

template <typename T>
struct is_setter : std::false_type
{
    const static bool arg_is_pointer = false;
};

template <typename T, typename Class>
struct is_setter<void (Class::*)(T)> : std::true_type
{
    using arg_type = T;
    const static bool arg_is_pointer = std::is_pointer<T>::value;
};

template <typename T>
struct is_clearable
{
    const static bool value = is_seq_container<T>::value;
};

template <typename K, typename V>
struct is_clearable<std::map<K, V>> : std::true_type
{
};

} // namespace bound

#endif