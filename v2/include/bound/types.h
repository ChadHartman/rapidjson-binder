#ifndef BOUND_TYPES_H_
#define BOUND_TYPES_H_

#include <string>
#include <map>

namespace bound
{

template <typename T>
using JsonProperties = std::map<std::string, T>;

struct JsonRaw
{
    std::string value;
    bool render = true;
};

struct JsonString
{
    std::string value;
    bool render = true;
};

struct JsonDouble
{
    double value;
    bool render = true;
};

struct JsonBool
{
    bool value;
    bool render = true;
};

struct JsonInt
{
    long long value;
    bool render = true;
};

struct JsonUint
{
    unsigned long long value;
    bool render = true;
};

} // namespace bound

#endif