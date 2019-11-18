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