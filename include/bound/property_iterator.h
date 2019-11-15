#ifndef BOUND_PROPERTY_ITERATOR_H_
#define BOUND_PROPERTY_ITERATOR_H_

#include "property.h"

namespace bound
{

template <typename T, T... S, typename F>
constexpr void for_sequence(std::integer_sequence<T, S...>, F &&f)
{
    using unpack_t = int[];
    (void)unpack_t{(static_cast<void>(f(std::integral_constant<T, S>{})), 0)..., 0};
}

template <typename T, typename F>
void ListProperties(T &object, F &&f)
{
    constexpr auto prop_count = std::tuple_size<decltype(T::BOUND_PROPS_NAME)>::value;
    for_sequence(std::make_index_sequence<prop_count>{}, [&](auto i) {
        constexpr auto property = std::get<i>(T::BOUND_PROPS_NAME);
        f(property);
    });
}

} // namespace bound

#endif