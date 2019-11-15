#ifndef BOUND_WRITE_GETTER_H_
#define BOUND_WRITE_GETTER_H_

#include "../property.h"

namespace bound
{
namespace write
{

template <typename T, typename M, typename F>
typename std::enable_if_t<std::is_member_object_pointer<M>::value>
Get(T &object, M member, F &&f)
{
    f(object.*(member));
}

template <typename T, typename M, typename F>
typename std::enable_if_t<is_getter<M>::value>
Get(T &object, M member, F &&f)
{
    auto value = (object.*(member))();
    f(value);
}

template <typename T, typename M, typename F>
typename std::enable_if_t<
    !std::is_member_object_pointer<M>::value &&
    !is_getter<M>::value>
Get(T &object, M member, F &&f)
{
    // Do nothing
}

} // namespace write
} // namespace bound

#endif