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

#ifndef BOUND_PROPERTY_H_
#define BOUND_PROPERTY_H_

#include <map>
#include "types.h"
#include "type_traits.h"

namespace bound
{

// Encapsulates a Class property
//  Adapted from Guillaume Racicot and fiorentinoing's StackOverflow answers:
//  https://stackoverflow.com/questions/34090638/c-convert-json-to-object
template <typename Class, typename T>
struct Property
{
    constexpr Property(T Class::*member, const char *name, bool render_name)
        : member{member},
          name{name},
          render_name{render_name} {}

    T Class::*member;
    // Can't use std::string because it is not instantiable in a constexpr
    const char *name;
    const bool render_name;
    const bool is_json_props = is_json_properties<T Class::*>::value;
};

template <typename Class, typename T>
constexpr auto property(T Class::*member, const char *name)
{
    return Property<Class, T>{member, name, true};
}

template <typename Class, typename T>
constexpr typename std::enable_if<
    std::is_member_object_pointer<JsonProperties<T> Class::*>::value,
    Property<Class, JsonProperties<T>>>::type
property(JsonProperties<T> Class::*member)
{
    return Property<Class, JsonProperties<T>>{member, "", false};
}

} // namespace bound

#endif