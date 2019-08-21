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

template <typename Stream>
class Reader
{
private:
    Parser<Stream> &parser_;
    ReadStatus &read_status_;

    template <typename T, typename M>
    typename std::enable_if_t<std::is_member_object_pointer<M>::value>
    Set(T &instance, M property)
    {
        Read(instance.*(property));
    }

    template <typename T, typename M>
    typename std::enable_if_t<is_setter<M>::value>
    Set(T &instance, M property)
    {
        typename is_setter<M>::arg_type value;
        Read(value);
        (instance.*(property))(value);
    }

    template <typename T, typename M>
    typename std::enable_if_t<
        !is_setter<M>::value &&
        !std::is_member_object_pointer<M>::value>
    Set(T &instance, M property)
    {
        // Do nothing
    }

    template <typename T>
    void FindProperty(T &instance, std::string &key)
    {
        ListProperties(instance, [&](auto &property) {
            if (property.name != key || !ReadTarget<decltype(property.member)>::is_assignable)
            {
                return;
            }

            Set(instance, property.member);
        });
    }

public:
    Reader(Parser<Stream> &parser, ReadStatus &read_status)
        : parser_{parser}, read_status_{read_status} {}

    template <typename T>
    typename std::enable_if_t<is_bound<T>::value>
    Read(T &instance)
    {
        std::string key;
        bool last_token_was_key = false;

        if (parser_.event().type == Event::kTypeBegin)
        {
            parser_.FetchNextEvent();
        }

        while (read_status_.success() && parser_.FetchNextEvent())
        {
            switch (parser_.event().type)
            {

            case Event::kTypeNull:
            case Event::kTypeBool:
            case Event::kTypeInt:
            case Event::kTypeUint:
            case Event::kTypeInt64:
            case Event::kTypeUint64:
            case Event::kTypeDouble:
            case Event::kTypeString:
            case Event::kTypeStartObject:
            case Event::kTypeStartArray:
                FindProperty(instance, key);
                last_token_was_key = false;
                break;

            case Event::kTypeKey:
                if (last_token_was_key)
                {
                    read_status_.set_error_message("Unassigned key: \"" + key + "\"; found new key: " + parser_.event().ToString());
                    return;
                }
                else
                {
                    last_token_was_key = true;
                    key = parser_.event().string_value;
                }
                break;

            case Event::kTypeEndObject:
            case Event::kTypeEnd:
                return;

            default:
                read_status_.set_error_message("Unexpected token=" + parser_.event().ToString());
                return;
            }
        }
    }

    template <typename T>
    typename std::enable_if_t<is_seq_container<T>::value>
    Read(T &instance)
    {
        if (parser_.event().type == Event::kTypeBegin)
        {
            parser_.FetchNextEvent();
        }

        while (read_status_.success() && parser_.FetchNextEvent())
        {
            switch (parser_.event().type)
            {
            case Event::kTypeBegin:
                break;

            case Event::kTypeKey:
            case Event::kTypeEndObject:
                read_status_.set_error_message("Unexpected event=" + parser_.event().ToString());
                return;

            case Event::kTypeNull:
            case Event::kTypeBool:
            case Event::kTypeInt:
            case Event::kTypeUint:
            case Event::kTypeInt64:
            case Event::kTypeUint64:
            case Event::kTypeDouble:
            case Event::kTypeString:
            case Event::kTypeStartObject:
            case Event::kTypeStartArray:
            {
                typename T::value_type child;
                Read(child);
                instance.push_back(child);
            }
            break;

            case Event::kTypeEndArray:
            case Event::kTypeEnd:
                return;
            }
        }

        read_status_.set_error_message("Reached unexpected point.");
    }

    template <typename T>
    typename std::enable_if_t<!is_bound<T>::value && !is_seq_container<T>::value>
    Read(T &instance)
    {
        switch (parser_.event().type)
        {
        case Event::kTypeNull:
            Assign(instance, nullptr, read_status_);
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

        return;
    }
};

template <typename T>
T FromJson(std::string &&json)
{
    T instance;
    Parser<rapidjson::StringStream> parser{rapidjson::StringStream(json.c_str())};
    ReadStatus read_status;
    Reader<rapidjson::StringStream>{parser, read_status}.Read(instance);
    return instance;
}

} // namespace read
} // namespace bound

#endif