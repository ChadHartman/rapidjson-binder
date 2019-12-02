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

#ifndef BOUND_WRITE_SCANNER_H_
#define BOUND_WRITE_SCANNER_H_

#include "../write_config.h"

namespace bound
{
namespace write
{

// Scans objects for whether they are to be written
class Scanner
{
private:
    const WriteConfig &write_config_;

public:
    Scanner(const WriteConfig &write_config) : write_config_{write_config} {}
    Scanner(const Scanner &) = delete;
    Scanner &operator=(const Scanner &) = delete;
    Scanner(const Scanner &&) = delete;
    Scanner &operator=(const Scanner &&) = delete;

    template <typename T>
    typename std::enable_if<is_bound<T>::value, unsigned>::type
    Scan(T &object)
    {
        unsigned count = 0;

        ListProperties(object, [&](auto property) {
            Get(object, property.member, [&](auto &value) {
                count += Scan(value);
            });
        });

        if (!write_config_.IsFilteringEmptyObjects())
        {
            // Return 1 for self when not filtering
            return count + 1;
        }

        // 1 for this object when it was properties
        return count == 0 ? 0 : count + 1;
    }

    template <typename T>
    unsigned Scan(std::map<std::string, T> &map)
    {
        unsigned count = 0;
        for (auto &i : map)
        {
            count += Scan(i.second);
        }

        if (!write_config_.IsFilteringEmptyObjects())
        {
            // Return 1 for self when not filtering
            return count + 1;
        }

        // 1 for this object when it was properties
        return count == 0 ? 0 : count + 1;
    }

    template <typename T>
    typename std::enable_if<is_seq_container<T>::value, unsigned>::type
    Scan(T &array)
    {
        unsigned count = 0;
        for (auto &i : array)
        {
            count += Scan(i);
        }

        if (!write_config_.IsFilteringEmptyArrays())
        {
            // Return 1 for self when not filtering
            return count + 1;
        }

        // 1 for this object when it was properties
        return count == 0 ? 0 : count + 1;
    }

    template <typename T>
    typename std::enable_if<std::is_pointer<T>::value, unsigned>::type
    Scan(const T pointer)
    {
        if (pointer == nullptr)
        {
            return write_config_.IsFilteringNullPointers() ? 0 : 1;
        }

        return Scan(*pointer);
    }

    template <typename T>
    typename std::enable_if<
        std::is_same<T, bool>::value ||
            std::is_same<T, std::string>::value ||
            is_int<T>::value ||
            is_uint<T>::value ||
            std::is_floating_point<T>::value,
        unsigned>::type
    Scan(const T value)
    {
        return write_config_.IsFiltered(value) ? 0 : 1;
    }

    unsigned Scan(const JsonRaw value)
    {
        return value.value.length() == 0 ? 0 : 1;
    }

    unsigned Scan(const JsonString value)
    {
        return value.render ? Scan(value.value) : 0;
    }

    unsigned Scan(const JsonFloat value)
    {
        return value.render ? Scan(value.value) : 0;
    }

    unsigned Scan(const JsonBool value)
    {
        return value.render ? Scan(value.value) : 0;
    }

    unsigned Scan(const JsonInt value)
    {
        return value.render ? Scan(value.value) : 0;
    }

    unsigned Scan(const JsonUint value)
    {
        return value.render ? Scan(value.value) : 0;
    }
};

} // namespace write
} // namespace bound

#endif