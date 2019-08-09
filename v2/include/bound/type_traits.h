#ifndef BOUND_TYPE_TRAITS_H_
#define BOUND_TYPE_TRAITS_H_

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

    static constexpr bool value = sizeof(test<T>(0)) == sizeof(char);
};

} // namespace bound

#endif