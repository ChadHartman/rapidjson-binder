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