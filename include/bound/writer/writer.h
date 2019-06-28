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

#ifndef BOUND_WRITER_WRITER_H_
#define BOUND_WRITER_WRITER_H_

#include <string>

#include <rapidjson/writer.h>

#include "../util.h"
#include "scanner.h"

namespace bound
{

namespace writer
{

// Writes out to rapidjson Writer
template <class Stream>
class Writer
{
private:
    rapidjson::Writer<Stream> &writer_;
    Scanner scanner_;

    // Setter property
    //  Setters are unused for serialization
    template <class O, typename M>
    inline typename std::enable_if_t<util::is_setter<M>::value>
    WriteProperty(O &object, const char *key, M member)
    {
        // Do nothing
    }

    // Getter propert
    //  Method invoked
    template <class O, typename M>
    inline typename std::enable_if_t<util::is_getter<M>::value>
    WriteProperty(O &object, const char *key, M member)
    {
        auto value = (object.*(member))();

        if (scanner_.Scan(value) > 0)
        {
            writer_.Key(key);
            Write(value);
        }
    }

    // Non-getter field
    //  Passes reference
    template <class O, typename M>
    inline typename std::enable_if_t<std::is_member_object_pointer<M>::value>
    WriteProperty(O &object, const char *key, M member)
    {
        auto &value = object.*(member);

        if (scanner_.Scan(value) > 0)
        {
            writer_.Key(key);
            Write(value);
        }
    }

public:
    Writer(
        rapidjson::Writer<Stream> &writer,
        WriteConfig &write_config)
        : writer_(writer),
          scanner_{write_config} {}

    template <typename T>
    typename std::enable_if_t<std::is_pointer<T>::value>
    Write(const T instance)
    {
        Write(*instance);
    }

    // Object with properties writer
    template <typename T>
    typename std::enable_if_t<
        !util::is_simple_property<T>::value &&
        !util::is_seq_container<T>::value &&
        !std::is_pointer<T>::value>
    Write(T &instance)
    {
        writer_.StartObject();

        constexpr auto prop_count = std::tuple_size<decltype(T::properties)>::value;
        util::for_sequence(std::make_index_sequence<prop_count>{}, [&](auto i) {
            constexpr auto property = std::get<i>(T::properties);
            WriteProperty(instance, property.name, property.member);
        });

        writer_.EndObject();
    }

    // Boolean writer
    //  Passed value can be from a class member or getter
    void Write(const bool instance)
    {
        writer_.Bool(instance);
    }

    // String writer
    //  Passed value can be from a class member or getter
    //  References allowed to minimize expensive copying
    template <class T>
    typename std::enable_if_t<std::is_same<typename std::remove_reference<T>::type, std::string>::value>
    Write(const T instance)
    {
        writer_.String(instance.c_str());
    }

    // String writer
    //  Passed value can be from a class member or getter
    //  References allowed to minimize expensive copying
    void Write(const char *s)
    {
        writer_.String(s);
    }

    // Unsigned int writer,
    //  Passed value can be from a class member or getter
    template <typename T>
    typename std::enable_if_t<util::is_unsigned_integer<T>::value>
    Write(const T instance)
    {
        writer_.Uint64(static_cast<uint64_t>(instance));
    }

    // Signed int writer
    //  Passed value can be from a class member or getter
    template <typename T>
    typename std::enable_if_t<util::is_integer<T>::value>
    Write(const T instance)
    {
        writer_.Int64(static_cast<int64_t>(instance));
    }

    // Floating point writer
    //  Passed value can be from a class member or getter
    template <typename T>
    typename std::enable_if<std::is_floating_point<T>::value>::type
    Write(const T instance)
    {
        writer_.Double(static_cast<double>(instance));
    }

    // Fundamental sequential container (vector, deque, list) types (bool, int, double, etc)
    template <template <typename...> class Container, typename T>
    typename std::enable_if<
        util::is_seq_container<Container<T>>::value &&
        std::is_fundamental<T>::value>::type
    Write(Container<T> &container)
    {
        writer_.StartArray();

        for (T item : container)
        {
            if (scanner_.Scan(item) > 0)
            {
                Write(item);
            }
        }

        writer_.EndArray();
    }

    // Object sequential container (vector, deque, list) types types (std::string, etc)
    template <template <typename...> class Container, typename T>
    typename std::enable_if<
        util::is_seq_container<Container<T>>::value &&
        !std::is_fundamental<T>::value>::type
    Write(Container<T> &container)
    {
        writer_.StartArray();

        for (T &item : container)
        {
            if (scanner_.Scan(item) > 0)
            {
                Write(item);
            }
        }

        writer_.EndArray();
    }
};

} // namespace writer

} // namespace bound

#endif