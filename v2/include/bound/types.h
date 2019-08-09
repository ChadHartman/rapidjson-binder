#ifndef BOUND_TYPES_H_
#define BOUND_TYPES_H_

#include <string>


namespace bound
{

struct JsonRaw
{
    std::string value;
};

struct JsonString
{
    std::string value;
};

struct JsonDouble
{
    double value;
};

struct JsonBool
{
    bool value;
};

struct JsonInt
{
    long long value;
};

struct JsonUint
{
    unsigned long long value;
};

} // namespace bound

#endif