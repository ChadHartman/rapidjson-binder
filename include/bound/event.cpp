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

#include "event.h"

#ifdef BOUND_EVENT_DEBUG
#include <stdio.h>
#endif

using namespace bound;
using namespace rapidjson;

Event::Type Event::type()
{
    return type_;
}

bool Event::Null()
{
#ifdef BOUND_EVENT_DEBUG
    printf(">>> Null\n");
#endif

    type_ = Type::kTypeNull;

    return true;
}

bool Event::Bool(bool b)
{
#ifdef BOUND_EVENT_DEBUG
    printf(">>> Bool %s\n", b ? "true" : "false");
#endif

    value_.bool_value = b;
    type_ = Type::kTypeBool;

    return true;
}

bool Event::Int(int i)
{
#ifdef BOUND_EVENT_DEBUG
    printf(">>> Int %d\n", i);
#endif

    value_.int_value = i;
    type_ = Type::kTypeInt;

    return true;
}

bool Event::Uint(unsigned u)
{
#ifdef BOUND_EVENT_DEBUG
    printf(">>> Uint %d\n", u);
#endif

    value_.unsigned_value = u;
    type_ = Type::kTypeUint;

    return true;
}

bool Event::Int64(int64_t i)
{
#ifdef BOUND_EVENT_DEBUG
    printf(">>> Int64 %lld\n", i);
#endif

    value_.int64_t_value = i;
    type_ = Type::kTypeInt64;

    return true;
}

bool Event::Uint64(uint64_t u)
{
#ifdef BOUND_EVENT_DEBUG
    printf(">>> Uint64 %llu\n", u);
#endif

    value_.uint64_t_value = u;
    type_ = Type::kTypeUint64;

    return true;
}

bool Event::Double(double d)
{
#ifdef BOUND_EVENT_DEBUG
    printf(">>> Double %f\n", d);
#endif

    value_.double_value = d;
    type_ = Type::kTypeDouble;

    return true;
}

bool Event::String(const char *str, rapidjson::SizeType length, bool copy)
{
#ifdef BOUND_EVENT_DEBUG
    printf(">>> String \"%s\"\n", str);
#endif

    string_value_ = str;
    type_ = Type::kTypeString;

    return true;
}

bool Event::StartObject()
{
#ifdef BOUND_EVENT_DEBUG
    printf(">>> StartObject\n");
#endif

    type_ = Type::kTypeStartObject;

    return true;
}

bool Event::Key(const char *str, rapidjson::SizeType length, bool copy)
{
#ifdef BOUND_EVENT_DEBUG
    printf(">>> Key \"%s\"\n", str);
#endif

    string_value_ = str;
    type_ = Type::kTypeKey;

    return true;
}

bool Event::EndObject(rapidjson::SizeType memberCount)
{
#ifdef BOUND_EVENT_DEBUG
    printf(">>> EndObject\n");
#endif

    type_ = Type::kTypeEndObject;

    return true;
}

bool Event::StartArray()
{
#ifdef BOUND_EVENT_DEBUG
    printf(">>> StartArray\n");
#endif

    type_ = Type::kTypeStartArray;

    return true;
}

bool Event::EndArray(rapidjson::SizeType elementCount)
{
#ifdef BOUND_EVENT_DEBUG
    printf(">>> EndArray\n");
#endif

    type_ = Type::kTypeEndArray;

    return true;
}

bool Event::SetValue(std::string &s)
{
    if (!(type_ & (kTypeString | kTypeKey)))
    {
        return false;
    }

    s = string_value_;
    return true;
}

bool Event::SetValue(bool &b)
{
    if (type_ != kTypeBool)
    {
        return false;
    }

    b = value_.bool_value;
    return true;
}