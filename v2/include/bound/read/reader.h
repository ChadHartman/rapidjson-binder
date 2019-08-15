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

template <template <typename...> class Container, typename T, typename Stream>
Container<T> Read(ReadContext<Container<T>, Stream> &ctx);

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