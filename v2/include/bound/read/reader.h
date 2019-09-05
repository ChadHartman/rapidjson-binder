#ifndef BOUND_READ_READER_H_
#define BOUND_READ_READER_H_

#include "parser.h"
#include "read_target.h"
#include "assign.h"
#include "../read_status.h"
#include "../type_traits.h"

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

    // Writer for JsonRaw properties
    void Write(rapidjson::Writer<rapidjson::StringBuffer> &writer)
    {
        do
        {
            switch (parser_.event().type)
            {
            case Event::kTypeNull:
                writer.Null();
                break;

            case Event::kTypeBool:
                writer.Bool(parser_.event().value.bool_value);
                break;

            case Event::kTypeInt:
                writer.Int(parser_.event().value.int_value);
                break;

            case Event::kTypeUint:
                writer.Uint(parser_.event().value.unsigned_value);
                break;

            case Event::kTypeInt64:
                writer.Int64(parser_.event().value.int64_t_value);
                break;

            case Event::kTypeUint64:
                writer.Uint64(parser_.event().value.uint64_t_value);
                break;

            case Event::kTypeDouble:
                writer.Double(parser_.event().value.double_value);
                break;

            case Event::kTypeString:
                writer.String(parser_.event().string_value.c_str());
                break;

            case Event::kTypeStartObject:
                writer.StartObject();
                parser_.FetchNextEvent();
                Write(writer);
                return;

            case Event::kTypeKey:
                writer.Key(parser_.event().string_value.c_str());
                break;

            case Event::kTypeStartArray:
                writer.StartArray();
                parser_.FetchNextEvent();
                Write(writer);
                return;

            case Event::kTypeEndArray:
                writer.EndArray();
                return;

            case Event::kTypeEndObject:
                writer.EndObject();
                return;
            }
        } while (parser_.FetchNextEvent());
    }

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
    typename std::enable_if_t<
        !is_setter<M>::value &&
        !std::is_member_object_pointer<M>::value>
    Set(T &instance, M property)
    {
        // Do nothing, needed for compilation
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
                    found = true;
                    Set(instance, property.member);
                }
            });
        }

        if (!found)
        {
            Skip();
        }
    }

    template <typename V>
    void SetProperty(JsonProperties<V> &instance, std::string &key)
    {
        V value;
        Read(value);
        if (read_status_.success())
        {
            instance[key] = value;
        }
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
        rapidjson::StringBuffer buffer;
        rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
        Write(writer);
        instance.value = buffer.GetString();
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

        Prime();

        switch (parser_.event().type)
        {
        case Event::kTypeNull:
            Assign(instance, null_ptr, read_status_);
            break;

        case Event::kTypeBool:
            Assign(instance, parser_.event().value.bool_value, read_status_);
            break;

        case Event::kTypeInt:
            Assign(instance, parser_.event().value.int_value, read_status_);
            break;

        case Event::kTypeUint:
            Assign(instance, parser_.event().value.unsigned_value, read_status_);
            break;

        case Event::kTypeInt64:
            Assign(instance, parser_.event().value.int64_t_value, read_status_);
            break;

        case Event::kTypeUint64:
            Assign(instance, parser_.event().value.uint64_t_value, read_status_);
            break;

        case Event::kTypeDouble:
            Assign(instance, parser_.event().value.double_value, read_status_);
            break;

        case Event::kTypeString:
            Assign(instance, parser_.event().string_value, read_status_);
            break;

        default:
            read_status_.set_error_message("Invalid read operation=" + parser_.event().ToString());
            break;
        }
    }
};

template <typename T>
T FromJson(std::string &json, ReadStatus &read_status)
{
    T instance;
    Parser<rapidjson::StringStream> parser{rapidjson::StringStream(json.c_str())};
    Reader<rapidjson::StringStream>{parser, read_status}.Read(instance);
    return instance;
}

template <typename T>
T FromJson(std::string &&json, ReadStatus &read_status)
{
    return FromJson<T>(json, read_status);
}

template <typename T>
T FromJson(std::string &&json)
{
    ReadStatus read_status;
    return FromJson<T>(json, read_status);
}

} // namespace read
} // namespace bound

#endif