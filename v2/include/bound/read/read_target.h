#ifndef BOUND_READ_READ_TARGET_H_
#define BOUND_READ_READ_TARGET_H_

namespace bound
{

namespace read
{

struct Unassignable
{
};

template <typename T>
struct raw_type
{
    using type =
        typename std::remove_pointer<
            typename std::remove_const<
                typename std::remove_reference<T>::type>::type>::type;
};

template <typename T>
struct ReadTarget
{
    using type = Unassignable;
    const static bool is_assignable = false;
};

template <typename T, typename Class>
struct ReadTarget<T (Class::*)()>
{
    using type = Unassignable;
    const static bool is_assignable = false;
};

template <typename T, typename Class>
struct ReadTarget<void (Class::*)(T)>
{
    using type = typename raw_type<T>::type;
    const static bool is_assignable = true;
};

template <typename T, typename Class>
struct ReadTarget<T(Class::*)>
{
    using type = typename raw_type<T>::type;
    const static bool is_assignable = std::is_member_object_pointer<T(Class::*)>::value;
};

} // namespace read

} // namespace bound

#endif