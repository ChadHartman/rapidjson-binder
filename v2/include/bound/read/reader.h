#ifndef BOUND_READ_READER_H_
#define BOUND_READ_READER_H_

#include "parser.h"
#include "../read_status.h"
#include "assign.h"
#include "read_target.h"
namespace bound
{

namespace read
{

template <typename T, typename Stream>
class Reader
{
private:
    Parser<Stream> &parser_;

    void Prime()
    {
        if (parser_.event().type == Event::kTypeBegin)
        {
            parser_.FetchNextEvent();
        }
    }

    template <typename V>
    typename std::enable_if<is_bound<V>::value, V>::type
    GetValue(ReadStatus &read_status)
    {
        return Reader<V, Stream>{parser_}.Read(read_status);
    }

    template <typename V>
    typename std::enable_if<!is_bound<V>::value, V>::type
    GetValue(ReadStatus &read_status)
    {
        V value;
        return value;
    }

public:
    Reader(Parser<Stream> &parser)
        : parser_{parser} {}
    Reader(const Reader &) = delete;
    Reader(const Reader &&) = delete;
    Reader &operator=(const Reader &) = delete;
    Reader &operator=(const Reader &&) = delete;

    T Read(ReadStatus &read_status)
    {
        T instance;
        std::string key;

        Prime();

        while (read_status.success() && parser_.FetchNextEvent())
        {
            bool last_token_was_key_ = false;

            switch (parser_.event().type)
            {
            case Event::kTypeBegin:
                break;

            case Event::kTypeNull:
                last_token_was_key_ = false;
                SetValue(instance, key, nullptr, read_status);
                break;

            case Event::kTypeBool:
                SetValue(instance, key, parser_.event().value.bool_value, read_status);
                last_token_was_key_ = false;
                break;

            case Event::kTypeInt:
                SetValue(instance, key, parser_.event().value.int_value, read_status);
                last_token_was_key_ = false;
                break;

            case Event::kTypeUint:
                SetValue(instance, key, parser_.event().value.unsigned_value, read_status);
                last_token_was_key_ = false;
                break;

            case Event::kTypeInt64:
                SetValue(instance, key, parser_.event().value.int64_t_value, read_status);
                last_token_was_key_ = false;
                break;

            case Event::kTypeUint64:
                SetValue(instance, key, parser_.event().value.uint64_t_value, read_status);
                last_token_was_key_ = false;
                break;

            case Event::kTypeDouble:
                SetValue(instance, key, parser_.event().value.double_value, read_status);
                last_token_was_key_ = false;
                break;

            case Event::kTypeString:
                SetValue(instance, key, parser_.event().string_value, read_status);
                last_token_was_key_ = false;
                break;

            case Event::kTypeKey:
                if (last_token_was_key_)
                {
                    read_status.set_error_message("Unassigned key: \"" + key + "\"; found new key: " + parser_.event().ToString());
                    return instance;
                }
                else
                {
                    last_token_was_key_ = true;
                    key = parser_.event().string_value;
                }
                break;

            case Event::kTypeStartObject:
            case Event::kTypeStartArray:
            {
                bool found = false;

                ListProperties(instance, [&](auto property) {
                    if (property.name != key)
                    {
                        return;
                    }

                    if (!ReadTarget<decltype(property.member)>::is_assignable)
                    {
                        return;
                    }

                    found = true;
                    using Type = typename ReadTarget<decltype(property.member)>::type;
                    SetValue(instance, key, GetValue<Type>(read_status), read_status);
                });

                if (!found)
                {
                    parser_.Skip();
                }

                last_token_was_key_ = false;
            }
            break;

            case Event::kTypeEndArray:
            case Event::kTypeEndObject:
            case Event::kTypeEnd:
                return instance;
            }
        }
        return instance;
    }
};

template <typename T>
T FromJson(std::string &&json)
{
    Parser<rapidjson::StringStream> parser{rapidjson::StringStream(json.c_str())};
    ReadStatus read_status;
    return Reader<T, rapidjson::StringStream>{parser}.Read(read_status);
}

} // namespace read

} // namespace bound

#endif