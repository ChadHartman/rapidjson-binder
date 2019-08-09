#ifndef BOUND_WRITE_WRITER_H_
#define BOUND_WRITE_WRITER_H_

#ifndef BOUND_FILE_WRITE_BUFFER_SIZE
#define BOUND_FILE_WRITE_BUFFER_SIZE 65536
#endif

#ifdef _WIN32
#define BOUND_BOUND_H_READ_MODE "rb"
#define BOUND_BOUND_H_WRITE_MODE "wb"
#else
#define BOUND_BOUND_H_READ_MODE "r"
#define BOUND_BOUND_H_WRITE_MODE "w"
#endif

#include <string>

#include <rapidjson/writer.h>
#include <rapidjson/filewritestream.h>

#include "../property_iterator.h"
#include "../type_traits.h"
#include "getter.h"

namespace bound
{

namespace write
{

template <class Stream>
class Writer
{
    rapidjson::Writer<Stream> &writer_;

public:
    Writer(rapidjson::Writer<Stream> &writer)
        : writer_{writer} {}

    template <typename T>
    typename std::enable_if_t<is_bound<T>::value>
    Write(T &object)
    {
        writer_.StartObject();

        ListProperties(object, [&](auto property) {
            writer_.Key(property.name);
            Get(object, property.member, [&](auto &value) {
                Write(value);
            });
        });

        writer_.EndObject();
    }

    template <typename T>
    typename std::enable_if_t<is_seq_container<T>::value>
    Write(T &array)
    {
        writer_.StartArray();

        for (auto &value : array)
        {
            Write(value);
        }

        writer_.EndArray();
    }

    void Write(const bool value)
    {
        writer_.Bool(value);
    }

    void Write(const std::string &value)
    {
        writer_.String(value.c_str());
    }
};

template <typename T>
std::string ToJson(T &instance)
{
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    Writer<rapidjson::StringBuffer>(writer).Write(instance);
    return buffer.GetString();
}

template <typename T>
bool ToJsonFile(T &instance, const std::string &&filename)
{
    FILE *fp = fopen(filename.c_str(), BOUND_BOUND_H_WRITE_MODE);
    if (fp)
    {
        char write_buffer[BOUND_FILE_WRITE_BUFFER_SIZE];
        rapidjson::FileWriteStream os(fp, write_buffer, sizeof(write_buffer));
        rapidjson::Writer<rapidjson::FileWriteStream> writer(os);
        Writer<rapidjson::FileWriteStream>(writer).Write(instance);
        fclose(fp);
        return true;
    }

    return false;
}

} // namespace write

} // namespace bound

#endif