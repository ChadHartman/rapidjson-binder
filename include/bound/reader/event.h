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

#ifndef BOUND_READER_EVENT_H_
#define BOUND_READER_EVENT_H_

// Enable this flag to get values set by rapidjson
//#define BOUND_READER_EVENT_H_DEBUG

#include "rapidjson/reader.h"
#include <string>

namespace bound
{

namespace reader
{

union Value {
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

    Value value{0};
    std::string string_value;
    Type type{Type::kTypeBegin};

    // === Rapidjson Setters === //
    bool Null()
    {
#ifdef BOUND_READER_EVENT_H_DEBUG
        printf("<Null/>\n");
#endif

        type = Type::kTypeNull;

        return true;
    }

    bool Bool(bool b)
    {
#ifdef BOUND_READER_EVENT_H_DEBUG
        printf("<Bool value=\"%s\"/>\n", b ? "true" : "false");
#endif

        value.bool_value = b;
        type = Type::kTypeBool;

        return true;
    }

    bool Int(int i)
    {
#ifdef BOUND_READER_EVENT_H_DEBUG
        printf("<Int value=\"%d\"/>\n", i);
#endif

        value.int_value = i;
        type = Type::kTypeInt;

        return true;
    }

    bool Uint(unsigned u)
    {
#ifdef BOUND_READER_EVENT_H_DEBUG
        printf("<Uint value=\"%d\"/>\n", u);
#endif

        value.unsigned_value = u;
        type = Type::kTypeUint;

        return true;
    }

    bool Int64(int64_t i)
    {
#ifdef BOUND_READER_EVENT_H_DEBUG
        printf("<Int64 value=\"%lld\"/>\n", i);
#endif

        value.int64_t_value = i;
        type = Type::kTypeInt64;

        return true;
    }

    bool Uint64(uint64_t u)
    {
#ifdef BOUND_READER_EVENT_H_DEBUG
        printf("<Uint64 value=\"%llu\"/>\n", u);
#endif

        value.uint64_t_value = u;
        type = Type::kTypeUint64;

        return true;
    }

    bool Double(double d)
    {
#ifdef BOUND_READER_EVENT_H_DEBUG
        printf("<Double value=\"%f\"/>\n", d);
#endif

        value.double_value = d;
        type = Type::kTypeDouble;

        return true;
    }

    bool String(const char *str, rapidjson::SizeType length, bool copy)
    {
#ifdef BOUND_READER_EVENT_H_DEBUG
        printf("<String value=\"%s\"/>\n", str);
#endif

        string_value = str;
        type = Type::kTypeString;

        return true;
    }

    bool StartObject()
    {
#ifdef BOUND_READER_EVENT_H_DEBUG
        printf("<Object>\n");
#endif

        type = Type::kTypeStartObject;

        return true;
    }

    bool Key(const char *str, rapidjson::SizeType length, bool copy)
    {
#ifdef BOUND_READER_EVENT_H_DEBUG
        printf("<Key value=\"%s\"/>\n", str);
#endif

        string_value = str;
        type = Type::kTypeKey;

        return true;
    }

    bool EndObject(rapidjson::SizeType memberCount)
    {
#ifdef BOUND_READER_EVENT_H_DEBUG
        printf("</Object>\n");
#endif

        type = Type::kTypeEndObject;

        return true;
    }

    bool End()
    {
        type = Type::kTypeEnd;
        return true;
    }

    bool StartArray()
    {
#ifdef BOUND_READER_EVENT_H_DEBUG
        printf("<Array>\n");
#endif

        type = Type::kTypeStartArray;

        return true;
    }

    bool EndArray(rapidjson::SizeType elementCount)
    {
#ifdef BOUND_READER_EVENT_H_DEBUG
        printf("</Array>\n");
#endif

        type = Type::kTypeEndArray;

        return true;
    }

    bool IsSimple() const
    {
        const static unsigned long simple_value =
            Event::kTypeNull |
            Event::kTypeBool |
            Event::kTypeInt |
            Event::kTypeUint |
            Event::kTypeInt64 |
            Event::kTypeUint64 |
            Event::kTypeDouble |
            Event::kTypeString;

        return type & simple_value;
    }

    const std::string ToString() const
    {
        std::string response;

        switch (type)
        {
        case Event::kTypeBegin:
            response = "{\"type\":\"Begin\"}";
            break;

        case Event::kTypeNull:
            response = "{\"type\":\"Null\"}";
            break;

        case Event::kTypeBool:
        {
            std::string bool_value{value.bool_value ? "true" : "false"};
            response = "{\"type\":\"Bool\",\"value\":" + bool_value + "}";
        }
        break;

        case Event::kTypeInt:
            response = "{\"type\":\"Int\",\"value\":" + std::to_string(value.int_value) + "}";
            break;

        case Event::kTypeUint:
            response = "{\"type\":\"Uint\",\"value\":" + std::to_string(value.unsigned_value) + "}";
            break;

        case Event::kTypeInt64:
            response = "{\"type\":\"Int64\",\"value\":" + std::to_string(value.int64_t_value) + "}";
            break;

        case Event::kTypeUint64:
            response = "{\"type\":\"Uint64\",\"value\":" + std::to_string(value.uint64_t_value) + "}";
            break;

        case Event::kTypeDouble:
            response = "{\"type\":\"Double\",\"value\":" + std::to_string(value.double_value) + "}";
            break;

        case Event::kTypeString:
            response = "{\"type\":\"String\",\"value\":\"" + string_value + "\"}";
            break;

        case Event::kTypeStartObject:
            response = "{\"type\":\"StartObject\"}";
            break;

        case Event::kTypeKey:
            response = "{\"type\":\"Key\",\"value\":\"" + string_value + "\"}";
            break;

        case Event::kTypeStartArray:
            response = "{\"type\":\"StartArray\"}";
            break;

        case Event::kTypeEndArray:
            response = "{\"type\":\"EndArray\"}";
            break;

        case Event::kTypeEndObject:
            response = "{\"type\":\"EndObject\"}";
            break;

        case Event::kTypeEnd:
            response = "{\"type\":\"End\"}";
            break;
        }
        return response;
    }
};

} // namespace reader

} // namespace bound

#endif
