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

#include "write/writer.h"
#include "read/reader.h"

namespace bound
{

template <typename T>
void FromJson(const std::string &json, T &object)
{
    ReadStatus status;
    bound::read::FromJson(json, object, status);
}

template <typename T>
void FromJson(const std::string &&json, T &object)
{
    FromJson(json, object);
}

template <typename T>
T FromJson(const std::string &json)
{
    T instance;
    FromJson(json, instance);
    return instance;
}

template <typename T>
T FromJson(const std::string &&json)
{
    return FromJson<T>(json);
}

template <typename T>
std::string ToJson(T &instance)
{
    return write::ToJson(instance);
}

} // namespace bound

#endif