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