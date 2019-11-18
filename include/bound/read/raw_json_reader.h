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

#ifndef BOUND_READ_RAW_JSON_READER_H_
#define BOUND_READ_RAW_JSON_READER_H_

namespace bound
{

namespace read
{

template <typename Stream>
class RawJsonReader
{
    Parser<Stream> &parser_;

public:
    RawJsonReader(Parser<Stream> &parser) : parser_{parser} {}

    // Writer for JsonRaw properties
    void Write(rapidjson::Writer<rapidjson::StringBuffer> &writer)
    {
        int depth = 0;

        do
        {
            switch (parser_.event().type)
            {
            case Event::kTypeNull:
                writer.Null();
                break;

            case Event::kTypeBool:
                writer.Bool(parser_.event().value.bool_value);
                break;

            case Event::kTypeInt:
                writer.Int(parser_.event().value.int_value);
                break;

            case Event::kTypeUint:
                writer.Uint(parser_.event().value.unsigned_value);
                break;

            case Event::kTypeInt64:
                writer.Int64(parser_.event().value.int64_t_value);
                break;

            case Event::kTypeUint64:
                writer.Uint64(parser_.event().value.uint64_t_value);
                break;

            case Event::kTypeDouble:
                writer.Double(parser_.event().value.double_value);
                break;

            case Event::kTypeString:
                writer.String(parser_.event().string_value.c_str());
                break;

            case Event::kTypeStartObject:
                writer.StartObject();
                ++depth;
                break;

            case Event::kTypeKey:
                writer.Key(parser_.event().string_value.c_str());
                break;

            case Event::kTypeStartArray:
                writer.StartArray();
                ++depth;
                break;

            case Event::kTypeEndArray:
                writer.EndArray();
                --depth;
                break;

            case Event::kTypeEndObject:
                writer.EndObject();
                --depth;
                break;

            case Event::kTypeBegin:
                continue;

            default:
                break;
            }

            if (depth == 0)
            {
                return;
            }

        } while (parser_.FetchNextEvent());
    }

    void Read(JsonRaw &instance)
    {
        rapidjson::StringBuffer buffer;
        rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
        Write(writer);
        instance.value = buffer.GetString();
    }
};

} // namespace read

} // namespace bound

#endif