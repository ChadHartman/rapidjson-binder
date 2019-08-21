#ifndef BOUND_READ_ARRAY_READER_H_
#define BOUND_READ_ARRAY_READER_H_

#include "parser.h"
#include "../read_status.h"
#include "assign.h"
#include "read_target.h"

namespace bound
{

namespace read
{

template <template <typename...> class Container, typename Stream, typename T, typename V>
void Add(ReadContext<Container<T>, Stream> &ctx, V &&item)
{
    T child;
    Assign(child, item, ctx.read_status);
    if (ctx.read_status.success())
    {
        ctx.instance.push_back(child);
    }
}

template <typename T, typename Stream>
typename std::enable_if<is_seq_container<T>::value, T>::type
Read(ReadContext<T, Stream> &ctx)
{
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

        case Event::kTypeKey:
        case Event::kTypeEndObject:
            ctx.read_status.set_error_message("Unexpected event=" + ctx.parser.event().ToString());
            return ctx.instance;

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
            ReadContext<typename T::value_type, Stream> child_ctx{ctx.parser, ctx.read_status};
            Add(ctx, Read<typename T::value_type>(child_ctx));
        }
        break;

        case Event::kTypeEndArray:
        case Event::kTypeEnd:
            return ctx.instance;
        }
    }

    ctx.read_status.set_error_message("Reached unexpected point.");

    return ctx.instance;
}

} // namespace read

} // namespace bound

#endif