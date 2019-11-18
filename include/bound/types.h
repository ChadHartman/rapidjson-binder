/*
Copyright 2019 Chad Hartman

Permission is hereby granted, free of charge, to any person obtaining a copy of 
this software and associated documentation files (the "Software"), to deal in the 
Software without restriction, including without limitation the rights to use, copy, 
modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, 
and to permit persons to whom the Software is furnished to do so, subject to the 
following conditions:

The above copyright notice and this permission notice shall be included in all 
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, 
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT 
HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION 
OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE 
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. 
*/

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

    JsonProperty<T> &operator=(const std::nullptr_t new_val)
    {
        value = T();
        return *this;
    }

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

    JsonString &operator=(const std::nullptr_t new_val)
    {
        value = "";
        return *this;
    }

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