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

#ifndef BOUND_READ_READER_H_
#define BOUND_READ_READER_H_

#include "parser.h"
#include "read_target.h"
#include "assign.h"
#include "read_status.h"
#include "../type_traits.h"
#include "raw_json_reader.h"

// #define BOUND_READ_READER_H_DEBUG

namespace bound
{
namespace read
{

template <typename Stream>
class Reader
{
private:
    const static unsigned int kEventTypeStartValue =
        Event::kTypeNull |
        Event::kTypeBool |
        Event::kTypeInt |
        Event::kTypeUint |
        Event::kTypeInt64 |
        Event::kTypeUint64 |
        Event::kTypeDouble |
        Event::kTypeString |
        Event::kTypeStartObject |
        Event::kTypeStartArray;

    Parser<Stream> &parser_;
    ReadStatus &read_status_;

    // Recusively skips unmapped sections of json
    void Skip()
    {
        if (parser_.event().IsSimple())
        {
            // Skipped
            return;
        }

        const static unsigned long start_complex = Event::kTypeStartArray | Event::kTypeStartObject;
        const static unsigned long end_complex = Event::kTypeEndArray | Event::kTypeEndObject;

        while (parser_.FetchNextEvent() && !(parser_.event().type & end_complex))
        {
            if (parser_.event().type & start_complex)
            {
                // Recurse lest we bail too early
                Skip();
            }
        }
    }

    // === Set values via member objer pointer or setter method === //

    // Set member object pointer directly
    template <typename T, typename M>
    typename std::enable_if_t<std::is_member_object_pointer<M>::value>
    Set(T &instance, M mop)
    {
        Read(instance.*(mop));
    }

    // Create instance, read into it, and call setter with pointer arg
    template <typename T, typename M>
    typename std::enable_if_t<
        is_setter<M>::value &&
        is_setter<M>::arg_is_pointer>
    Set(T &instance, M property)
    {
        typename ReadTarget<M>::type value;
        Read(value);
        if (read_status_.success())
        {
            (instance.*(property))(&value);
        }
    }

    // Create instance, read into it, and call setter without pointer arg
    template <typename T, typename M>
    typename std::enable_if_t<
        is_setter<M>::value &&
        !is_setter<M>::arg_is_pointer>
    Set(T &instance, M property)
    {
        typename ReadTarget<M>::type value;
        Read(value);
        if (read_status_.success())
        {
            (instance.*(property))(value);
        }
    }

    // Does nothing, exists only for compilation but will never be called
    template <typename T, typename M>
    typename std::enable_if_t<
        !is_setter<M>::value &&
        !std::is_member_object_pointer<M>::value>
    Set(T &instance, M property)
    {
        // Do nothing, needed for compilation
    }

    // === Property setters for map and bound objects === //

    // Set property of map
    template <typename T>
    void SetProperty(std::map<std::string, T> &instance, std::string &key)
    {
        T value;
        Read(value);
        if (read_status_.success())
        {
            instance[key] = value;
        }
    }

    // Set property of bound object
    template <typename T>
    typename std::enable_if_t<is_bound<T>::value>
    SetProperty(T &instance, std::string &key)
    {
        bool found = false;

        // Find explicitly defined property
        ListProperties(instance, [&](auto &property) {
            // property.name == "" when it is a json_props map
            if (property.name != key || !ReadTarget<decltype(property.member)>::is_assignable)
            {
                return;
            }

            found = true;
            Set(instance, property.member);
        });

        // Explicitly defined property not found, find dynamic key collection
        if (!found)
        {
            ListProperties(instance, [&](auto &property) {
                if (property.is_json_props)
                {
                    SetProperty(instance, property.member, key);
                    found = true;
                }
            });
        }

        if (!found)
        {
            Skip();
        }
    }

    // Set property of child bound object/map
    template <typename T, typename M>
    typename std::enable_if_t<
        is_bound<T>::value &&
        std::is_member_object_pointer<M>::value>
    SetProperty(T &instance, M member, std::string &key)
    {
        SetProperty(instance.*(member), key);
    }

    template <typename T, typename M>
    typename std::enable_if_t<
        !is_bound<T>::value ||
        !std::is_member_object_pointer<M>::value>
    SetProperty(T &instance, M member, std::string &key)
    {
        // Needed for compilation
    }

    template <typename T>
    typename std::enable_if_t<
        !is_bound<T>::value &&
        !is_json_properties<T>::value>
    SetProperty(T &instance, std::string &key)
    {
        // Needed for compilation
    }

    void Prime()
    {
        if (parser_.event().type == Event::kTypeBegin)
        {
            parser_.FetchNextEvent();
        }
    }

    template <typename T>
    inline typename std::enable_if_t<is_clearable<T>::value>
    Clear(T &container)
    {
        container.clear();
    }

    template <typename T>
    inline typename std::enable_if_t<!is_clearable<T>::value>
    Clear(T &container)
    {
        // Needed for compilation
    }

public:
    Reader(Parser<Stream> &parser, ReadStatus &read_status)
        : parser_{parser},
          read_status_{read_status} {}
    Reader(const Reader &) = delete;
    Reader &operator=(const Reader &) = delete;

    template <typename T>
    typename std::enable_if_t<
        is_bound<T>::value ||
        is_json_properties<T>::value>
    Read(T &instance)
    {
#ifdef BOUND_READ_READER_H_DEBUG
        printf("Reader#Read[bound|json_properties]\n");
#endif
        std::string key;
        bool last_token_was_key = false;
        Event::Type event_type;

        Prime();
        // Reset for a clean slate; only works for maps
        Clear(instance);

        while (read_status_.success() && parser_.FetchNextEvent())
        {
            event_type = parser_.event().type;

            if (event_type & kEventTypeStartValue)
            {
                if (!last_token_was_key)
                {
                    read_status_.error_message = "No key found for " + parser_.event().ToString();
                    break;
                }

                SetProperty(instance, key);
                last_token_was_key = false;
                continue;
            }

            if (event_type == Event::kTypeKey)
            {
                if (last_token_was_key)
                {
                    read_status_.error_message = "Unassigned key: \"" + key + "\"; found new key: " + parser_.event().ToString();
                    break;
                }

                last_token_was_key = true;
                key = parser_.event().string_value;
                continue;
            }

            if (event_type != Event::kTypeEndObject)
            {
                read_status_.error_message = "Unexpected token=" + parser_.event().ToString();
            }

            break;
        }
    }

    template <typename T>
    typename std::enable_if_t<is_seq_container<T>::value>
    Read(T &instance)
    {
#ifdef BOUND_READ_READER_H_DEBUG
        printf("Reader#Read[seq_container]\n");
#endif
        Event::Type event_type;

        Prime();
        // Reset for clean slate
        Clear(instance);

        while (read_status_.success() && parser_.FetchNextEvent())
        {
            event_type = parser_.event().type;

            if (event_type & kEventTypeStartValue)
            {
                typename T::value_type child;
                Read(child);
                if (read_status_.success())
                {
                    instance.push_back(child);
                }
                continue;
            }
            else if (event_type != Event::kTypeEndArray)
            {
                read_status_.error_message = "Unexpected event=" + parser_.event().ToString();
            }

            break;
        }
    }

    // Read into JsonRaw object, then assign it to instance
    template <typename T>
    std::enable_if_t<std::is_assignable<T, JsonRaw>::value>
    Read(T &instance)
    {
#ifdef BOUND_READ_READER_H_DEBUG
        printf("Reader#Read[is_assignable:JsonRaw]\n");
#endif
        JsonRaw json_raw;
        RawJsonReader<Stream>(parser_).Read(json_raw);
        if (!Assign(instance, json_raw))
        {
            read_status_.error_message = "Cannot assign \"" + json_raw.value + "\" to " + typeid(T).name() + ".";
        }
    }

    // Read into JsonRaw object
    void Read(JsonRaw &instance)
    {
#ifdef BOUND_READ_READER_H_DEBUG
        printf("Reader#Read[JsonRaw]\n");
#endif
        RawJsonReader<Stream>(parser_).Read(instance);
    }

    // Simple object assignment
    template <typename T>
    typename std::enable_if_t<
        !is_bound<T>::value &&
        !is_seq_container<T>::value &&
        !is_json_properties<T>::value &&
        !std::is_same<T, JsonRaw>::value &&
        !std::is_assignable<T, JsonRaw>::value>
    Read(T &instance)
    {
#ifdef BOUND_READ_READER_H_DEBUG
        printf("Reader#Read[!bound&!seq_container&!json_properties]\n");
#endif
        // Assignment operations require an lvalue ref
        const static auto null_ptr = nullptr;

        bool assigned = false;
        Prime();

        switch (parser_.event().type)
        {
        case Event::kTypeNull:
            assigned = Assign(instance, null_ptr);
            break;

        case Event::kTypeBool:
            assigned = Assign(instance, parser_.event().value.bool_value);
            break;

        case Event::kTypeInt:
            assigned = Assign(instance, parser_.event().value.int_value);
            break;

        case Event::kTypeUint:
            assigned = Assign(instance, parser_.event().value.unsigned_value);
            break;

        case Event::kTypeInt64:
            assigned = Assign(instance, parser_.event().value.int64_t_value);
            break;

        case Event::kTypeUint64:
            assigned = Assign(instance, parser_.event().value.uint64_t_value);
            break;

        case Event::kTypeDouble:
            assigned = Assign(instance, parser_.event().value.double_value);
            break;

        case Event::kTypeString:
            assigned = Assign(instance, parser_.event().string_value);
            break;

        default:
            break;
        }

        if (!assigned)
        {
            read_status_.error_message =
                "Cannot assign type \"" + std::string(typeid(instance).name()) +
                "\" with event " + parser_.event().ToString() + ".";
        }
    }
};

template <typename T>
const ReadStatus FromJson(const std::string &json, T &instance)
{
    ReadStatus status;
    Parser<rapidjson::StringStream> parser{rapidjson::StringStream(json.c_str())};
    Reader<rapidjson::StringStream>{parser, status}.Read(instance);
    return status;
}

} // namespace read
} // namespace bound

#endif