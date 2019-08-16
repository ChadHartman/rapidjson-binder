#ifndef BOUND_READ_READER_H_
#define BOUND_READ_READER_H_

#include "parser.h"
#include "../read_status.h"
#include "../type_traits.h"

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
typename std::enable_if<is_seq_container<T>::value, T>::type
Read(ReadContext<T, Stream> &ctx);

template <typename T, typename Stream>
typename std::enable_if<
    !is_bound<T>::value && !is_seq_container<T>::value,
    T>::type
Read(ReadContext<T, Stream> &ctx)
{
    printf("Shallow Read\n");

    switch (ctx.parser.event().type)
    {
    case Event::kTypeNull:
        Assign(ctx.instance, nullptr, ctx.read_status);
        break;
    case Event::kTypeBool:
        Assign(ctx.instance, ctx.parser.event().value.bool_value, ctx.read_status);
        break;
    case Event::kTypeInt:
        Assign(ctx.instance, ctx.parser.event().value.int_value, ctx.read_status);
        break;
    case Event::kTypeUint:
        Assign(ctx.instance, ctx.parser.event().value.unsigned_value, ctx.read_status);
        break;
    case Event::kTypeInt64:
        Assign(ctx.instance, ctx.parser.event().value.int64_t_value, ctx.read_status);
        break;
    case Event::kTypeUint64:
        Assign(ctx.instance, ctx.parser.event().value.uint64_t_value, ctx.read_status);
        break;
    case Event::kTypeDouble:
        Assign(ctx.instance, ctx.parser.event().value.double_value, ctx.read_status);
        break;
    case Event::kTypeString:
        Assign(ctx.instance, ctx.parser.event().string_value, ctx.read_status);
        break;
    default:
        ctx.read_status.set_error_message("Invalid read operation=" + ctx.parser.event().ToString());
        break;
    }

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