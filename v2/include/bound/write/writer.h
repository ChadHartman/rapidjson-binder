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

#include <rapidjson/prettywriter.h>
#include <rapidjson/filewritestream.h>

#include "../property_iterator.h"
#include "../type_traits.h"
#include "../types.h"
#include "getter.h"
#include "scanner.h"
#include "../write_config.h"

namespace bound
{

namespace write
{

template <typename W>
class Writer
{
    W &writer_;
    Scanner scanner_;

    template <typename T>
    void WriteMapContents(std::map<std::string, T> &object)
    {
        for (auto &i : object)
        {
            if (scanner_.Scan(i.second) == 0)
            {
                continue;
            }
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
    Writer(W &writer, const WriteConfig &write_config)
        : writer_{writer}, scanner_{write_config} {}
    Writer(const Writer &) = delete;
    Writer(const Writer &&) = delete;
    Writer &operator=(const Writer &) = delete;
    Writer &operator=(const Writer &&) = delete;

    template <typename T>
    typename std::enable_if_t<is_bound<T>::value>
    Write(T &object)
    {
        writer_.StartObject();

        ListProperties(object, [&](auto property) {
            Get(object, property.member, [&](auto &value) {
                if (scanner_.Scan(value) == 0)
                {
                    return;
                }

                if (property.is_json_props)
                {
                    WriteMapContents(value);
                }
                else
                {
                    writer_.Key(property.name);
                    Write(value);
                }
            });
        });

        writer_.EndObject();
    }

    template <typename T>
    void Write(std::map<std::string, T> &map)
    {
        writer_.StartObject();
        WriteMapContents(map);
        writer_.EndObject();
    }

    template <typename T>
    typename std::enable_if_t<is_seq_container<T>::value>
    Write(T &array)
    {
        writer_.StartArray();

        for (auto &value : array)
        {
            if (scanner_.Scan(value) > 0)
            {
                Write(value);
            }
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

    void Write(const JsonString value)
    {
        Write(value.value);
    }

    void Write(const JsonFloat value)
    {
        Write(value.value);
    }

    void Write(const JsonBool value)
    {
        Write(value.value);
    }

    void Write(const JsonInt value)
    {
        Write(value.value);
    }

    void Write(const JsonUint value)
    {
        Write(value.value);
    }
};

template <typename Stream, typename T>
inline void Write(Stream &writer, T &instance, const WriteConfig &write_config)
{
    writer.SetMaxDecimalPlaces(write_config.GetMaxDecimalPlaces());
    Writer<decltype(writer)>(writer, write_config).Write(instance);
}

template <typename T>
std::string ToJson(T &instance, const WriteConfig &write_config)
{
    rapidjson::StringBuffer buffer;

    if (write_config.HasPrefix())
    {
        rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
        writer.SetIndent(write_config.GetPrefix()[0], write_config.GetPrefix().length());
        Write(writer, instance, write_config);
    }
    else
    {
        rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
        Write(writer, instance, write_config);
    }

    return buffer.GetString();
}

template <typename T>
inline std::string ToJson(T &instance, const WriteConfig &&write_config)
{
    return ToJson(instance, write_config);
}

template <typename T>
bool ToJsonFile(T &instance, const std::string &&filename, const WriteConfig &&write_config)
{
    FILE *fp = fopen(filename.c_str(), BOUND_BOUND_H_WRITE_MODE);
    if (!fp)
    {
        return false;
    }

    char buffer[BOUND_FILE_WRITE_BUFFER_SIZE];
    rapidjson::FileWriteStream os(fp, buffer, sizeof(buffer));

    if (write_config.HasPrefix())
    {
        rapidjson::PrettyWriter<decltype(os)> writer(os);
        writer.SetIndent(write_config.GetPrefix()[0], write_config.GetPrefix().length());
        Write(writer, instance, write_config);
    }
    else
    {
        rapidjson::Writer<decltype(os)> writer(os);
        Write(writer, instance, write_config);
    }

    fclose(fp);
    return true;
}

} // namespace write

} // namespace bound

#endif