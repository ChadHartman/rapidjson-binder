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

template <template <typename...> class Container, typename V>
struct is_json_properties<Container<V>>
{
    const static bool value = std::is_same<Container<V>, JsonProperties<V>>::value;
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
struct is_getter<T (Class::*)()> : std::true_type
{
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

} // namespace bound

#endif