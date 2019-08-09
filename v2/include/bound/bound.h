#ifndef BOUND_BOUND_H_
#define BOUND_BOUND_H_

#define BOUND_MAJOR_VERSION 2
#define BOUND_MINOR_VERSION 0
#define BOUND_PATCH_VERSION 0
#define BOUND_VERSION_STRING "2.0.0"

#ifndef BOUND_PROPS_NAME
#define BOUND_PROPS_NAME properties
#endif

#include <string>

#include "read/parser.h"
#include "write/writer.h"

namespace bound
{

template <typename T>
void FromJson(const std::string &json, T &object)
{
    read::Parser<rapidjson::StringStream> parser{rapidjson::StringStream(json.c_str())};
}

template <typename T>
void FromJson(const std::string &&json, T &object)
{
    return FromJson(json, object);
}

template <typename T>
std::string ToJson(T &object)
{
    return write::ToJson(object);
}

} // namespace bound

#endif