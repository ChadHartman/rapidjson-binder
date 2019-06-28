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

#ifndef BOUND_READER_READER_H_
#define BOUND_READER_READER_H_

#include <map>

#include "parser.h"
#include "../read_status.h"
#include "../util.h"

namespace bound
{

namespace reader
{

template <typename A, typename B>
struct is_assignable
{
    enum
    {
        value = std::is_same<A, B>::value ||
                std::is_assignable<A, B>::value
    };
};

template <typename A, typename B>
struct is_convertable
{
    enum
    {
        value = !is_assignable<A, B>::value &&
                std::is_arithmetic<A>::value &&
                std::is_arithmetic<B>::value
    };
};

template <typename A, typename B>
struct is_unassignable
{
    enum
    {
        value = !is_assignable<A, B>::value &&
                !is_convertable<A, B>::value
    };
};

template <typename T>
struct setter_arg
{
    const static bool is_pointer = std::is_pointer<
        typename util::function_traits<T>::template arg<0>::type>::value;

    using type = typename util::raw_type<
        typename util::function_traits<T>::template arg<0>::type>::type;
};

// Reads a value from the JSON Parser into Object Properties
template <class Stream>
class Reader
{
private:
    Parser<Stream> &parser_;
    bool last_token_was_key_ = false;

    // Readies the parser to the correct position for iterative parsing
    //  Ignores Event::kTypeBegin, and skips to the next token
    //  Compares that token to target_type
    // param status - The status to write errors to
    // param target_type - The desired initial token
    void Prime(ReadStatus &status, Event::Type target_type)
    {
        if (parser_.event().type == Event::kTypeBegin)
        {
            parser_.FetchNextEvent();
        }

        if (parser_.event().type != target_type)
        {
            status.set_error_message("Invalid token: " + parser_.event().ToString());
        }
    }

    // === array operations === //

    // Add simple item to array
    template <template <typename...> class Container, typename T, typename V>
    void Add(Container<T> &array, V &item, ReadStatus &status)
    {
        T child;
        Assign(child, item, status);
        if (status.success())
        {
            array.push_back(child);
        }
    }

    // Add complex item (located in parser) to array
    template <template <typename...> class Container, typename T>
    void Add(Container<T> &array, ReadStatus &status)
    {
        T child;
        Read(child, status);
        array.push_back(child);
    }

    // === object operations === //

    // Simple assign
    template <typename A, typename B>
    typename std::enable_if_t<is_assignable<A, B>::value>
    Assign(A &a, B &b, ReadStatus &status)
    {
        a = b;
    }

    // Conversion assign
    template <typename A, typename B>
    typename std::enable_if_t<is_convertable<A, B>::value>
    Assign(A &a, B &b, ReadStatus &status)
    {
        a = static_cast<A>(b);
    }

    // Incompatible assignment
    template <typename A, typename B>
    typename std::enable_if_t<is_unassignable<A, B>::value>
    Assign(A &a, B &b, ReadStatus &status)
    {
        status.set_error_message(
            "Cannot assign event: " +
            parser_.event().ToString() +
            " type \"" +
            typeid(A).name() +
            "\" to type \"" +
            typeid(B).name() + "\".");
    }

    // --- object simple setters --- //

    // Getter Setter for simple value (does nothing)
    template <typename M, typename O, typename V>
    typename std::enable_if_t<util::is_getter<M>::value>
    SetValue(O &object, M member, V &value, ReadStatus &status)
    {
        // getters unused to reading
    }

    // Setter setter for simple value
    //  Creates an instance of the setter arg, writes to that,
    //  then passes the arg into the setter
    template <typename M, typename O, typename V>
    typename std::enable_if_t<util::is_setter<M>::value>
    SetValue(O &object, M member, V &value, ReadStatus &status)
    {
        using ArgType = typename setter_arg<M>::type;

        ArgType arg;
        Assign(arg, value, status);

        if (status.success())
        {
            CallSetter(object, member, arg);
        }
    }

    // Property Setter for simple value
    //  Calls Assign using the property
    template <typename M, typename O, typename V>
    typename std::enable_if_t<std::is_member_object_pointer<M>::value>
    SetValue(O &object, M member, V &value, ReadStatus &status)
    {
        Assign(object.*(member), value, status);
    }

    // --- object complex setters --- //

    // Calls setter requesting a pointer arg
    template <typename M, typename O, typename V>
    typename std::enable_if_t<setter_arg<M>::is_pointer>
    CallSetter(O &object, M member, V &value)
    {
        (object.*(member))(&value);
    }

    // Calls setter requesting non-pointer
    template <typename M, typename O, typename V>
    typename std::enable_if_t<!setter_arg<M>::is_pointer>
    CallSetter(O &object, M member, V &value)
    {
        (object.*(member))(value);
    }

    // Getter setter for complex values (does nothing)
    template <typename M, typename O>
    typename std::enable_if_t<util::is_getter<M>::value>
    SetValue(O &object, M member, ReadStatus &status)
    {
        // getters unused to reading
    }

    // Setter Setter for complex values
    //  Creates an instance, recursively reads into it,
    //  and passes it into the setter
    template <typename M, typename O>
    typename std::enable_if_t<util::is_setter<M>::value>
    SetValue(O &object, M member, ReadStatus &status)
    {
        using ArgType = typename setter_arg<M>::type;

        ArgType arg;
        Read(arg, status);

        if (status.success())
        {
            CallSetter(object, member, arg);
        }
    }

    // Property Setter for complex values
    //  Calls recursive read on the property itself
    template <typename M, typename O>
    typename std::enable_if_t<std::is_member_object_pointer<M>::value>
    SetValue(O &object, M member, ReadStatus &status)
    {
        Read(object.*(member), status);
    }

    // --- object property setters --- //

    // Locate the object's property with a given name
    template <typename O, typename V>
    void SetProperty(O &object, std::string &key, V &value, ReadStatus &status)
    {
        if (!last_token_was_key_)
        {
            status.set_error_message("Value was found with no preceeding key: " + parser_.event().ToString());
            return;
        }

        constexpr auto prop_count = std::tuple_size<decltype(O::properties)>::value;
        bool found = false;
        util::for_sequence(std::make_index_sequence<prop_count>{}, [&](auto i) {
            constexpr auto property = std::get<i>(O::properties);
            if (key != property.name)
            {
                return;
            }

            found = true;
            SetValue(object, property.member, value, status);

            if (!status.success())
            {
                status.set_error_message("Unable to set property \"" + key + "\"; " + status.error_message());
            }
        });

        if (!found)
        {
            status.set_error_message("Object does not have property with name \"" + key + "\".");
        }
    }

    template <typename O>
    void SetProperty(O &object, std::string &key, ReadStatus &status)
    {
        if (!last_token_was_key_)
        {
            status.set_error_message("Value was found with no preceeding key: " + parser_.event().ToString());
            return;
        }

        constexpr auto prop_count = std::tuple_size<decltype(O::properties)>::value;
        bool found = false;
        util::for_sequence(std::make_index_sequence<prop_count>{}, [&](auto i) {
            constexpr auto property = std::get<i>(O::properties);
            if (key != property.name)
            {
                return;
            }

            found = true;
            SetValue(object, property.member, status);

            if (!status.success())
            {
                status.set_error_message("Unable to set property \"" + key + "\"; " + status.error_message());
            }
        });

        if (!found)
        {
            status.set_error_message("Object does not have property with name \"" + key + "\".");
        }
    }

    // Simple Property reader, needs to exist to compile
    template <typename T>
    typename std::enable_if_t<util::is_simple_property<T>::value>
    Read(T &instance, ReadStatus &status)
    {
        status.set_error_message("Attempted read of simple property.");
    }

public:
    Reader(Parser<Stream> &parser)
        : parser_{parser} {}

    // Object
    template <typename T>
    typename std::enable_if_t<
        !util::is_seq_container<T>::value &&
        !util::is_simple_property<T>::value>
    Read(T &instance, ReadStatus &status)
    {
        std::string key;
        last_token_was_key_ = false;

        Prime(status, Event::kTypeStartObject);

        while (status.success() && parser_.FetchNextEvent())
        {
            switch (parser_.event().type)
            {
            case Event::kTypeBegin:
            case Event::kTypeNull:
                last_token_was_key_ = false;
                break;

            case Event::kTypeBool:
                SetProperty(instance, key, parser_.event().value.bool_value, status);
                last_token_was_key_ = false;
                break;

            case Event::kTypeInt:
                SetProperty(instance, key, parser_.event().value.int_value, status);
                last_token_was_key_ = false;
                break;

            case Event::kTypeUint:
                SetProperty(instance, key, parser_.event().value.unsigned_value, status);
                last_token_was_key_ = false;
                break;

            case Event::kTypeInt64:
                SetProperty(instance, key, parser_.event().value.int64_t_value, status);
                last_token_was_key_ = false;
                break;

            case Event::kTypeUint64:
                SetProperty(instance, key, parser_.event().value.uint64_t_value, status);
                last_token_was_key_ = false;
                break;

            case Event::kTypeDouble:
                SetProperty(instance, key, parser_.event().value.double_value, status);
                last_token_was_key_ = false;
                break;

            case Event::kTypeString:
                SetProperty(instance, key, parser_.event().string_value, status);
                last_token_was_key_ = false;
                break;

            case Event::kTypeStartObject:
                SetProperty(instance, key, status);
                last_token_was_key_ = false;
                break;

            case Event::kTypeKey:
                if (last_token_was_key_)
                {
                    status.set_error_message("Unassigned key: \"" + key + "\"; found new key: " + parser_.event().ToString());
                    return;
                }
                else
                {
                    last_token_was_key_ = true;
                    key = parser_.event().string_value;
                }
                break;

            case Event::kTypeStartArray:
                SetProperty(instance, key, status);
                last_token_was_key_ = false;
                break;

            case Event::kTypeEndArray:
                status.set_error_message("Unexpected event while parsing object: " + parser_.event().ToString());
                return;

            case Event::kTypeEndObject:
            case Event::kTypeEnd:
                return;
            }
        }
    }

    template <template <typename...> class Container, typename T>
    typename std::enable_if_t<util::is_seq_container<Container<T>>::value>
    Read(Container<T> &instance, ReadStatus &status)
    {
        Prime(status, Event::kTypeStartArray);

        while (status.success() && parser_.FetchNextEvent())
        {
            switch (parser_.event().type)
            {
            case Event::kTypeBegin:
            case Event::kTypeNull:
                break;

            case Event::kTypeBool:
                Add(instance, parser_.event().value.bool_value, status);
                break;

            case Event::kTypeInt:
                Add(instance, parser_.event().value.int_value, status);
                break;

            case Event::kTypeUint:
                Add(instance, parser_.event().value.unsigned_value, status);
                break;

            case Event::kTypeInt64:
                Add(instance, parser_.event().value.int64_t_value, status);
                break;

            case Event::kTypeUint64:
                Add(instance, parser_.event().value.uint64_t_value, status);
                break;

            case Event::kTypeDouble:
                Add(instance, parser_.event().value.double_value, status);
                break;

            case Event::kTypeString:
                Add(instance, parser_.event().string_value, status);
                break;

            case Event::kTypeStartObject:
                Add(instance, status);
                break;

            case Event::kTypeKey:
            case Event::kTypeEndObject:
                status.set_error_message("Unexpected event while parsing array: " + parser_.event().ToString());
                return;

            case Event::kTypeStartArray:
                Add(instance, status);
                break;

            case Event::kTypeEndArray:
            case Event::kTypeEnd:
                return;
            }
        }
    }
};

} // namespace reader

} // namespace bound

#endif