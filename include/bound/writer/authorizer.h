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

#ifndef BOUND_WRITER_AUTHORIZER_H_
#define BOUND_WRITER_AUTHORIZER_H_

#include <string>

namespace bound
{

namespace writer
{

// Queries whether a class's property should be written
template <typename T>
class Authorizer
{
private:
    bool (T::*authorizer_method_)(const char *) = NULL;
    int writable_count_ = 0;

    void set_authorizer_method(bool (T::*method)(const char *))
    {
        authorizer_method_ = method;
    }

    // Overload needs to exist to compile
    template <typename M>
    typename std::enable_if_t<!util::is_authorizer<M>::value>
    set_authorizer_method(M method) {}

public:
    T &instance;

    Authorizer(T &instance)
        : instance{instance}
    {
        constexpr auto prop_count = std::tuple_size<decltype(T::BOUND_PROPS_NAME)>::value;
        util::for_sequence(std::make_index_sequence<prop_count>{}, [&](auto i) {
            constexpr auto property = std::get<i>(T::BOUND_PROPS_NAME);
            set_authorizer_method(property.member);
        });
    }

    // Calls the authorizer if it exists
    bool Authorize(const char *name)
    {
        return authorizer_method_ == NULL || (instance.*(authorizer_method_))(name);
    }
};

} // namespace writer
} // namespace bound

#endif