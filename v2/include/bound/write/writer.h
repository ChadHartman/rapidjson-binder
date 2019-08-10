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
#include "../types.h"
#include "getter.h"
#include "../write_config.h"

namespace bound
{

namespace write
{

template <class Stream>
class Writer
{
    rapidjson::Writer<Stream> &writer_;

    template <typename T>
    void WriteMapContents(std::map<std::string, T> &object)
    {
        for (auto &i : object)
        {
            writer_.Key(i.first.c_str());
            Write(i.second);
        }
    }

    template <typename T>
    void WriteMapContents(T &object)
    {
        // Exists only for compilation
    }

public:
    Writer(rapidjson::Writer<Stream> &writer)
        : writer_{writer} {}

    template <typename T>
    typename std::enable_if_t<is_bound<T>::value>
    Write(T &object)
    {
        writer_.StartObject();

        ListProperties(object, [&](auto property) {
            Get(object, property.member, [&](auto &value) {
                if (property.render_name)
                {
                    writer_.Key(property.name);
                    Write(value);
                }
                else
                {
                    WriteMapContents(value);
                }
            });
        });

        writer_.EndObject();
    }

    template <typename T>
    void Write(std::map<std::string, T> &object)
    {

        writer_.StartObject();
        WriteMapContents(object);
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

    template <typename T>
    typename std::enable_if_t<std::is_pointer<T>::value>
    Write(const T pointer)
    {
        if (pointer == NULL)
        {
            writer_.Null();
        }
        else
        {
            Write(*pointer);
        }
    }

    template <typename T>
    typename std::enable_if_t<std::is_same<T, bool>::value>
    Write(const T value)
    {
        writer_.Bool(value);
    }

    template <typename T>
    typename std::enable_if_t<is_int<T>::value>
    Write(const T value)
    {
        writer_.Int64(value);
    }

    template <typename T>
    typename std::enable_if_t<is_uint<T>::value>
    Write(const T value)
    {
        writer_.Uint64(value);
    }

    template <typename T>
    typename std::enable_if_t<std::is_floating_point<T>::value>
    Write(const T value)
    {
        writer_.Double(value);
    }

    void Write(const std::string &value)
    {
        writer_.String(value.c_str());
    }

    void Write(const JsonRaw value)
    {
        writer_.RawValue(
            value.value.c_str(),
            value.value.length(),
            rapidjson::kStringType);
    }

    template <typename T>
    typename std::enable_if_t<
        std::is_convertible<T, JsonString>::value ||
        std::is_convertible<T, JsonDouble>::value ||
        std::is_convertible<T, JsonBool>::value ||
        std::is_convertible<T, JsonInt>::value ||
        std::is_convertible<T, JsonUint>::value>
    Write(const T value)
    {
        Write(value.value);
    }
};

template <typename T>
std::string ToJson(T &instance, const WriteConfig &&write_config)
{
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    writer.SetMaxDecimalPlaces(write_config.GetMaxDecimalPlaces());
    Writer<rapidjson::StringBuffer>(writer).Write(instance);
    return buffer.GetString();
}

template <typename T>
bool ToJsonFile(T &instance, const std::string &&filename, const WriteConfig &&write_config)
{
    FILE *fp = fopen(filename.c_str(), BOUND_BOUND_H_WRITE_MODE);
    if (fp)
    {
        char write_buffer[BOUND_FILE_WRITE_BUFFER_SIZE];
        rapidjson::FileWriteStream os(fp, write_buffer, sizeof(write_buffer));
        rapidjson::Writer<rapidjson::FileWriteStream> writer(os);
        writer.SetMaxDecimalPlaces(write_config.GetMaxDecimalPlaces());
        Writer<rapidjson::FileWriteStream>(writer).Write(instance);
        fclose(fp);
        return true;
    }

    return false;
}

} // namespace write

} // namespace bound

#endif