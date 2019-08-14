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
struct ReadTarget<T(Class::*)>
{
    using type = T;
    const static bool is_assignable = true;
};

template <typename T, typename Class>
struct ReadTarget<void (Class::*)(T)>
{
    using type = T;
    const static bool is_assignable = true;
};


} // namespace read

} // namespace bound

#endif