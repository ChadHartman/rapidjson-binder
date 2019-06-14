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

#ifndef BOUND_WRITER_H_
#define BOUND_WRITER_H_

#include <string>
#include <rapidjson/writer.h>
#include "util.h"
#include <vector>
#include <array>

namespace bound
{

namespace writer
{

// Forward declaration for object writing
template <class Stream, typename T>
typename std::enable_if<!std::is_fundamental<T>::value && !std::is_same<T, std::string>::value>::type
Write(rapidjson::Writer<Stream> &writer, T &instance);

// Boolean writer
//  Passed value can be from a class member or getter
template <class Stream>
void Write(rapidjson::Writer<Stream> &writer, bool instance)
{
    writer.Bool(instance);
}

// String writer
//  Passed value can be from a class member or getter
//  References allowed to minimize expensive copying
template <class Stream, class T>
typename std::enable_if<std::is_same<T, std::string>::value || std::is_same<T, std::string &>::value>::type
Write(rapidjson::Writer<Stream> &writer, T instance)
{
    writer.String(instance.c_str());
}

// Unsigned int writer,
//  Floats are considered "signed", so integral check is omitted
//  Passed value can be from a class member or getter
template <class Stream, typename T>
typename std::enable_if<std::is_unsigned<T>::value && !std::is_same<T, bool>::value>::type
Write(rapidjson::Writer<Stream> &writer, T instance)
{
    writer.Uint64(static_cast<uint64_t>(instance));
}

// Signed int writer
//  bools are considered "unsigned", so bool checkes are omitted
//  Passed value can be from a class member or getter
template <class Stream, typename T>
typename std::enable_if<std::is_signed<T>::value && std::is_integral<T>::value>::type
Write(rapidjson::Writer<Stream> &writer, T instance)
{

    writer.Int64(static_cast<int64_t>(instance));
}

// Floating point writer
//  Passed value can be from a class member or getter
template <class Stream, typename T>
typename std::enable_if<std::is_floating_point<T>::value>::type
Write(rapidjson::Writer<Stream> &writer, T instance)
{
    writer.Double(static_cast<double>(instance));
}

// Fundamental sequential container (vector, deque, list) types (bool, int, double, etc)
template <class Stream, template <typename...> class Container, typename T>
typename std::enable_if<
    util::is_seq_container<Container<T>>::value &&
    std::is_fundamental<T>::value>::type
Write(rapidjson::Writer<Stream> &writer, Container<T> &container)
{
    writer.StartArray();

    for (T item : container)
    {
        Write(writer, item);
    }

    writer.EndArray();
}

// Object sequential container (vector, deque, list) types types (std::string, etc)
template <class Stream, template <typename...> class Container, typename T>
typename std::enable_if<
    util::is_seq_container<Container<T>>::value &&
    !std::is_fundamental<T>::value>::type
Write(rapidjson::Writer<Stream> &writer, Container<T> &container)
{
    writer.StartArray();

    for (T &item : container)
    {
        Write(writer, item);
    }

    writer.EndArray();
}

// Setter property
//  Setters are unused for serialization
template <class Stream, class O, class Class, typename T>
inline typename std::enable_if<util::is_setter<T Class::*>::value>::type
WriteProperty(rapidjson::Writer<Stream> &writer, O &object, const char *key, T Class::*member)
{
    // Do nothing
}

// Getter propert
//  Method invoked
template <class Stream, class O, class Class, typename T>
inline typename std::enable_if<util::is_getter<T Class::*>::value>::type
WriteProperty(rapidjson::Writer<Stream> &writer, O &object, const char *key, T Class::*member)
{
    writer.Key(key);

    using Type = typename util::function_traits<decltype(member)>::return_type;

    Type value = (object.*(member))();
    Write(writer, value);
}

// Non-getter field
//  Passes reference
template <class Stream, class O, class Class, typename T>
inline typename std::enable_if<!std::is_member_function_pointer<T Class::*>::value>::type
WriteProperty(rapidjson::Writer<Stream> &writer, O &object, const char *key, T Class::*member)
{
    writer.Key(key);
    Write(writer, object.*(member));
}

// Object with properties writer
template <class Stream, typename T>
typename std::enable_if<!std::is_fundamental<T>::value && !std::is_same<T, std::string>::value>::type
Write(rapidjson::Writer<Stream> &writer, T &instance)
{
    writer.StartObject();

    constexpr auto nbProperties = std::tuple_size<decltype(T::properties)>::value;
    util::for_sequence(std::make_index_sequence<nbProperties>{}, [&](auto i) {
        constexpr auto property = std::get<i>(T::properties);
        WriteProperty(writer, instance, property.name, property.member);
    });

    writer.EndObject();
}

} // namespace writer

} // namespace bound

#endif