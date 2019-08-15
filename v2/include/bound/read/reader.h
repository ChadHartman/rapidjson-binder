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
struct ReadContext
{
    T instance;
    Parser<Stream> &parser;
    ReadStatus &read_status;

    ReadContext(Parser<Stream> &parser, ReadStatus &read_status)
        : parser{parser}, read_status{read_status} {}
};

template <typename T, typename Stream>
typename std::enable_if<is_bound<T>::value, T>::type
Read(ReadContext<T, Stream> &ctx);

template <typename T, typename Stream>
typename std::enable_if<!is_bound<T>::value, T>::type
Read(ReadContext<T, Stream> &ctx);

template <typename T, typename Stream>
typename std::enable_if_t<is_bound<T>::value>
ReadComplex(ReadContext<T, Stream> &ctx, std::string &key)
{
    ListProperties(ctx.instance, [&](auto &property) {
        if (property.name != key || !ReadTarget<decltype(property.member)>::is_assignable)
        {
            return;
        }

        using Type = typename ReadTarget<decltype(property.member)>::type;
        ReadContext<Type, Stream> child_ctx{ctx.parser, ctx.read_status};
        Type value = Read<Type>(child_ctx);
        SetValue(ctx.instance, key, value, ctx.read_status);
    });
}

template <typename T, typename Stream>
typename std::enable_if_t<!is_bound<T>::value>
ReadComplex(ReadContext<T, Stream> &ctx, std::string &key)
{
    // Needed for compilation
}

template <typename T, typename Stream>
typename std::enable_if<is_bound<T>::value, T>::type
Read(ReadContext<T, Stream> &ctx)
{
    std::string key;
    bool last_token_was_key = false;

    if (ctx.parser.event().type == Event::kTypeBegin)
    {
        ctx.parser.FetchNextEvent();
    }

    while (ctx.read_status.success() && ctx.parser.FetchNextEvent())
    {
        switch (ctx.parser.event().type)
        {
        case Event::kTypeBegin:
            break;

        case Event::kTypeNull:
            last_token_was_key = false;
            SetValue(ctx.instance, key, nullptr, ctx.read_status);
            break;

        case Event::kTypeBool:
            SetValue(ctx.instance, key, ctx.parser.event().value.bool_value, ctx.read_status);
            last_token_was_key = false;
            break;

        case Event::kTypeInt:
            SetValue(ctx.instance, key, ctx.parser.event().value.int_value, ctx.read_status);
            last_token_was_key = false;
            break;

        case Event::kTypeUint:
            SetValue(ctx.instance, key, ctx.parser.event().value.unsigned_value, ctx.read_status);
            last_token_was_key = false;
            break;

        case Event::kTypeInt64:
            SetValue(ctx.instance, key, ctx.parser.event().value.int64_t_value, ctx.read_status);
            last_token_was_key = false;
            break;

        case Event::kTypeUint64:
            SetValue(ctx.instance, key, ctx.parser.event().value.uint64_t_value, ctx.read_status);
            last_token_was_key = false;
            break;

        case Event::kTypeDouble:
            SetValue(ctx.instance, key, ctx.parser.event().value.double_value, ctx.read_status);
            last_token_was_key = false;
            break;

        case Event::kTypeString:
            SetValue(ctx.instance, key, ctx.parser.event().string_value, ctx.read_status);
            last_token_was_key = false;
            break;

        case Event::kTypeKey:
            if (last_token_was_key)
            {
                ctx.read_status.set_error_message("Unassigned key: \"" + key + "\"; found new key: " + ctx.parser.event().ToString());
                return ctx.instance;
            }
            else
            {
                last_token_was_key = true;
                key = ctx.parser.event().string_value;
            }
            break;

        case Event::kTypeStartObject:
        case Event::kTypeStartArray:
            ReadComplex(ctx, key);
            last_token_was_key = false;
            break;

        case Event::kTypeEndArray:
        case Event::kTypeEndObject:
        case Event::kTypeEnd:
            return ctx.instance;
        }
    }
    return ctx.instance;
}

template <typename T, typename Stream>
typename std::enable_if<!is_bound<T>::value, T>::type
Read(ReadContext<T, Stream> &ctx)
{
    return ctx.instance;
}

template <typename T>
T FromJson(std::string &&json)
{
    Parser<rapidjson::StringStream> parser{rapidjson::StringStream(json.c_str())};
    ReadStatus read_status;
    ReadContext<T, rapidjson::StringStream> ctx{parser, read_status};
    return Read<T>(ctx);
}

} // namespace read

} // namespace bound

#endif