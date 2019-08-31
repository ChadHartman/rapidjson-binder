#ifndef BOUND_TYPES_H_
#define BOUND_TYPES_H_

#include <string>
#include <map>

namespace bound
{

template <typename T>
using JsonProperties = std::map<std::string, T>;

template <typename T>
struct JsonProperty
{
    T value;

    JsonProperty<T> &operator=(const T &new_val)
    {
        value = new_val;
        return *this;
    }

    template <typename V>
    typename std::enable_if<std::is_arithmetic<V>::value, JsonProperty<T> &>::type
    operator=(const T &new_val)
    {
        value = static_cast<T>(new_val);
        return *this;
    }

    bool render = true;
};

typedef JsonProperty<std::string> JsonString;
typedef JsonProperty<double> JsonFloat;
typedef JsonProperty<bool> JsonBool;
typedef JsonProperty<uint64_t> JsonUint;
typedef JsonProperty<int64_t> JsonInt;

struct JsonRaw
{
    std::string value;
    bool render = true;
};

} // namespace bound

#endif