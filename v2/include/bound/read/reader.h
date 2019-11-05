#ifndef BOUND_READ_READER_H_
#define BOUND_READ_READER_H_

#include "parser.h"
#include "read_target.h"
#include "assign.h"
#include "../read_status.h"
#include "../type_traits.h"
#include "raw_json_reader.h"

namespace bound
{
namespace read
{

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

template <typename Stream>
class Reader
{
private:
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

    template <typename T, typename M>
    typename std::enable_if_t<std::is_member_object_pointer<M>::value>
    Set(T &instance, M property)
    {
        Read(instance.*(property));
    }

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

    template <typename T, typename M>
    typename std::enable_if_t<std::is_member_object_pointer<M>::value>
    Set(T &instance, std::string &key, M property)
    {
        Read(instance.*(property));
    }

    template <typename T, typename M>
    typename std::enable_if_t<
        !is_setter<M>::value &&
        !std::is_member_object_pointer<M>::value>
    Set(T &instance, M property)
    {
        // Do nothing, needed for compilation
    }

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

    template <typename T, typename M>
    typename std::enable_if_t<is_bound<T>::value && std::is_member_object_pointer<M>::value>
    SetProperty(T &instance, M member, std::string &key)
    {
        SetProperty(instance.*(member), key);
    }

    template <typename T>
    typename std::enable_if_t<is_bound<T>::value>
    SetProperty(T &instance, std::string &key)
    {
        bool found = false;

        ListProperties(instance, [&](auto &property) {
            if (property.name != key || !ReadTarget<decltype(property.member)>::is_assignable)
            {
                return;
            }

            found = true;
            Set(instance, property.member);
        });

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

public:
    Reader(Parser<Stream> &parser, ReadStatus &read_status)
        : parser_{parser},
          read_status_{read_status} {}

    template <typename T>
    typename std::enable_if_t<is_bound<T>::value || is_json_properties<T>::value>
    Read(T &instance)
    {
        std::string key;
        bool last_token_was_key = false;
        Event::Type event_type;

        Prime();

        while (read_status_.success() && parser_.FetchNextEvent())
        {
            event_type = parser_.event().type;

            if (event_type & kEventTypeStartValue)
            {
                if (!last_token_was_key)
                {
                    read_status_.set_error_message("No key found for " + parser_.event().ToString());
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
                    read_status_.set_error_message("Unassigned key: \"" + key + "\"; found new key: " + parser_.event().ToString());
                    break;
                }

                last_token_was_key = true;
                key = parser_.event().string_value;
                continue;
            }

            if (event_type != Event::kTypeEndObject)
            {
                read_status_.set_error_message("Unexpected token=" + parser_.event().ToString());
            }

            break;
        }
    }

    template <typename T>
    typename std::enable_if_t<is_seq_container<T>::value>
    Read(T &instance)
    {
        Event::Type event_type;

        Prime();

        instance.clear();

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
                read_status_.set_error_message("Unexpected event=" + parser_.event().ToString());
            }

            break;
        }
    }

    void Read(JsonRaw &instance)
    {
        RawJsonReader<Stream>(parser_).Read(instance);
    }

    template <typename T>
    typename std::enable_if_t<
        !is_bound<T>::value &&
        !is_seq_container<T>::value &&
        !is_json_properties<T>::value>
    Read(T &instance)
    {
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
            std::string error_message = "Cannot assign type \"";
            error_message += typeid(instance).name();
            error_message += "\" with event ";
            error_message += parser_.event().ToString();
            error_message += ".";
            read_status_.set_error_message(error_message);
        }
    }
};

template <typename T>
void FromJson(const std::string &json, T &instance, ReadStatus &read_status)
{
    Parser<rapidjson::StringStream> parser{rapidjson::StringStream(json.c_str())};
    Reader<rapidjson::StringStream>{parser, read_status}.Read(instance);
}

} // namespace read
} // namespace bound

#endif