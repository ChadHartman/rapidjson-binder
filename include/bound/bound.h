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

#ifndef BOUND_BOUND_H_
#define BOUND_BOUND_H_

#define BOUND_MAJOR_VERSION 1
#define BOUND_MINOR_VERSION 1
#define BOUND_PATCH_VERSION 0
#define BOUND_VERSION_STRING "1.1.0"

#ifdef _WIN32
#define BOUND_BOUND_H_READ_MODE "rb"
#define BOUND_BOUND_H_WRITE_MODE "wb"
#else
#define BOUND_BOUND_H_READ_MODE "r"
#define BOUND_BOUND_H_WRITE_MODE "w"
#endif

#include <rapidjson/filewritestream.h>
#include <rapidjson/filereadstream.h>
#include <rapidjson/stringbuffer.h>

#include "property.h"
#include "writer/writer.h"
#include "reader/reader.h"

namespace bound
{

// Read from json string.
// Example:
//      User user;
//      std::string json = "{}";
//      ReadJson(json, user);
// Returns a ReadStatus instance.
template <class T>
//typename std::enable_if<util::is_object<T>::value, const ReadStatus>::type
const ReadStatus
FromJson(std::string &json, T &object)
{
    ReadStatus status;
    reader::Parser<rapidjson::StringStream> parser{rapidjson::StringStream(json.c_str())};
    reader::Reader<rapidjson::StringStream>(parser).Read(object, status);
    return status;
}

// Read from json file.
// Example:
//      User user;
//      ReadJson("user.json", user);
// Returns a ReadStatus instance.
template <class T>
const ReadStatus FromJsonFile(std::string filename, T &object)
{
    ReadStatus status;
    char buffer[65536];
    FILE *file = fopen(filename.c_str(), BOUND_BOUND_H_READ_MODE);
    if (file)
    {
        reader::Parser<rapidjson::FileReadStream> parser{rapidjson::FileReadStream(file, buffer, sizeof(buffer))};
        reader::Reader<rapidjson::FileReadStream>{parser}.Read(object, status);
        fclose(file);
    }
    else
    {
        status.set_error_message("Unable to open file \"" + filename + "\".");
    }
    return status;
}

// Write to string.
// Example:
//    std::string user_json = Write(user);
//  Returns the json string.
template <typename T>
inline std::string ToJson(T &instance)
{
    WriteConfig write_config;
    return ToJson(instance, write_config);
}

// Write to string.
// Example:
//    std::string user_json = Write(user);
//  Returns the json string.
template <typename T>
std::string ToJson(T &instance, WriteConfig &write_config)
{
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    writer::Writer<rapidjson::StringBuffer>(writer, write_config).Write(instance);
    return buffer.GetString();
}

// Write to some path.
// Example:
//      Write(user, "assets/data/user.json");
//  Returns whether the write was successful.
template <typename T>
inline bool ToJsonFile(T &instance, std::string filename)
{
    WriteConfig write_config;
    return ToJsonFile(instance, filename, write_config);
}

// Write to some path.
// Example:
//      Write(user, "assets/data/user.json");
//  Returns whether the write was successful.
template <typename T>
bool ToJsonFile(T &instance, std::string filename, WriteConfig &write_config)
{
    FILE *fp = fopen(filename.c_str(), BOUND_BOUND_H_WRITE_MODE);
    if (fp)
    {
        char write_buffer[65536];
        rapidjson::FileWriteStream os(fp, write_buffer, sizeof(write_buffer));
        rapidjson::Writer<rapidjson::FileWriteStream> writer(os);
        writer::Writer<rapidjson::FileWriteStream>(writer, write_config).Write(instance);
        fclose(fp);
        return true;
    }

    return false;
}

} // namespace bound

#endif