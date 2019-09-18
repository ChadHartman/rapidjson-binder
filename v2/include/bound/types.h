#ifndef BOUND_TYPES_H_
#define BOUND_TYPES_H_

#include <string>
#include <map>

namespace bound
{

template <typename T>
struct JsonProperty
{
    using type = T;

    T value;

    JsonProperty<T> &operator=(const T &new_val)
    {
        value = new_val;
        return *this;
    }

    template <typename V>
    typename std::enable_if<std::is_arithmetic<V>::value, JsonProperty<T> &>::type
    operator=(const V &new_val)
    {
        value = static_cast<T>(new_val);
        return *this;
    }

    bool render = true;
};

typedef JsonProperty<double> JsonFloat;
typedef JsonProperty<bool> JsonBool;
typedef JsonProperty<uint64_t> JsonUint;
typedef JsonProperty<int64_t> JsonInt;

struct JsonString
{
    using type = std::string;

    std::string value;

    JsonString &operator=(const std::string &new_val)
    {
        value = new_val;
        return *this;
    }

    JsonString &operator=(const bool &new_val)
    {
        value = new_val ? "true" : "false";
        return *this;
    }

    template <typename V>
    typename std::enable_if<std::is_arithmetic<V>::value, JsonString &>::type
    operator=(const V &new_val)
    {
        value = std::to_string(new_val);
        return *this;
    }

    bool render = true;
};

struct JsonRaw
{
    std::string value;
    bool render = true;
};

} // namespace bound

#endif