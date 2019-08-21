#ifndef BOUND_READ_OBJECT_READER_H_
#define BOUND_READ_OBJECT_READER_H_

#include "parser.h"
#include "../read_status.h"
#include "assign.h"
#include "read_target.h"
#include "reader.h"

namespace bound
{

namespace read
{

template <typename T, typename Stream, typename M, typename V>
typename std::enable_if_t<std::is_member_object_pointer<M>::value>
Set(ReadContext<T, Stream> &ctx, M property, V &&value)
{
    ctx.instance.*(property) = value;
}

template <typename T, typename Stream, typename M, typename V>
typename std::enable_if_t<is_setter<M>::value>
Set(ReadContext<T, Stream> &ctx, M property, V &&value)
{
    (ctx.instance.*(property))(value);
}

template <typename T, typename Stream, typename M, typename V>
typename std::enable_if_t<
    !is_setter<M>::value &&
    !std::is_member_object_pointer<M>::value>
Set(ReadContext<T, Stream> &ctx, M property, V &&value)
{
    // Do nothing
}

template <typename T, typename Stream>
void FindProperty(ReadContext<T, Stream> &ctx, std::string &key)
{
    ListProperties(ctx.instance, [&](auto &property) {
        if (property.name != key || !ReadTarget<decltype(property.member)>::is_assignable)
        {
            return;
        }

        using ChildType = typename ReadTarget<decltype(property.member)>::type;
        ReadContext<ChildType, Stream> child_ctx{ctx.parser, ctx.read_status};
        Set(ctx, property.member, Read<ChildType>(child_ctx));
    });
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
            FindProperty(ctx, key);
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

        case Event::kTypeEndObject:
        case Event::kTypeEnd:
            return ctx.instance;

        default:
            ctx.read_status.set_error_message("Unexpected token=" + ctx.parser.event().ToString());
            return ctx.instance;
        }
    }

    return ctx.instance;
}

} // namespace read

} // namespace bound

#endif