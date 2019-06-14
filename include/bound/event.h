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

#ifndef BOUND_EVENT_H_
#define BOUND_EVENT_H_

// Enable this flag to get values set by rapidjson
//#define BOUND_EVENT_DEBUG

#include "rapidjson/reader.h"
#include <string>

namespace bound
{

union FundamentalValue {
    bool bool_value;
    int int_value;
    unsigned unsigned_value;
    int64_t int64_t_value;
    uint64_t uint64_t_value;
    double double_value;
};

// Event which saves the current state of json document parsing.
struct Event : public rapidjson::BaseReaderHandler<rapidjson::UTF8<>, Event>
{
public:
    enum Type
    {
        kTypeBegin = 0,
        kTypeNull = 1 << 0,
        kTypeBool = 1 << 1,
        kTypeInt = 1 << 2,
        kTypeUint = 1 << 3,
        kTypeInt64 = 1 << 4,
        kTypeUint64 = 1 << 5,
        kTypeDouble = 1 << 6,
        kTypeString = 1 << 7,
        kTypeStartObject = 1 << 8,
        kTypeKey = 1 << 9,
        kTypeEndObject = 1 << 10,
        kTypeStartArray = 1 << 11,
        kTypeEndArray = 1 << 12,
        kTypeEnd = 1 << 13
    };

    Type type();

    // Rapidjson Setters
    bool Null();
    bool Bool(bool b);
    bool Int(int i);
    bool Uint(unsigned u);
    bool Int64(int64_t i);
    bool Uint64(uint64_t u);
    bool Double(double d);
    bool String(const char *str, rapidjson::SizeType length, bool copy);
    bool StartObject();
    bool Key(const char *str, rapidjson::SizeType length, bool copy);
    bool EndObject(rapidjson::SizeType memberCount);
    bool StartArray();
    bool EndArray(rapidjson::SizeType elementCount);

    // Object Mapping Setters
    bool SetValue(std::string &s);
    bool SetValue(bool &b);

    // Integer setter
    template <typename T>
    typename std::enable_if<std::is_integral<T>::value && !std::is_same<T, bool>::value, bool>::type
    SetValue(T &i)
    {
        switch (type_)
        {
        case kTypeInt:
            i = static_cast<T>(value_.int_value);
            return true;
        case kTypeUint:
            i = static_cast<T>(value_.unsigned_value);
            return true;
        case kTypeInt64:
            i = static_cast<T>(value_.int64_t_value);
            return true;
        case kTypeUint64:
            i = static_cast<T>(value_.uint64_t_value);
            return true;
        default:
            return false;
        }
    }

    template <typename T>
    typename std::enable_if<std::is_floating_point<T>::value, bool>::type
    SetValue(T &f)
    {
        if (type_ != kTypeDouble)
        {
            return false;
        }

        f = static_cast<T>(value_.double_value);
        return true;
    }

    // Empty implementation needed to compile, should be unreachable
    template <typename T>
    typename std::enable_if<!std::is_fundamental<T>::value && !std::is_same<T, std::string>::value, bool>::type
    SetValue(T &f)
    {
        return false;
    }

private:
    FundamentalValue value_{0};
    std::string string_value_;
    Type type_{Type::kTypeBegin};
};

} // namespace bound

#endif
