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

#define BOUND_MAJOR_VERSION 2
#define BOUND_MINOR_VERSION 0
#define BOUND_PATCH_VERSION 0
#define BOUND_VERSION_STRING "2.0.0"

#ifndef BOUND_PROPS_NAME
#define BOUND_PROPS_NAME properties
#endif

#include <string>

#include "write/writer.h"
#include "read/reader.h"

namespace bound
{

template <typename T>
struct CreateStatus
{
    T instance;
    const bool success;
    const std::string error_message;
};

template <typename T>
CreateStatus<T> CreateWithJson(const std::string &json)
{
    ReadStatus status;
    T instance;
    bound::read::FromJson(json, instance, status);
    return CreateStatus<T>{instance, status.success(), status.error_message()};
}

template <typename T>
inline CreateStatus<T> CreateWithJson(const std::string &&json)
{
    return CreateWithJson<T>(json);
}

template <typename T>
ReadStatus UpdateWithJson(T &instance, const std::string &json)
{
    ReadStatus status;
    bound::read::FromJson(json, instance, status);
    return status;
}

template <typename T>
inline ReadStatus UpdateWithJson(T &instance, const std::string &&json)
{
    return UpdateWithJson(instance, json);
}

template <typename T>
inline const std::string ToJson(T &instance, WriteConfig &config)
{
    return write::ToJson(instance, config);
}

template <typename T>
inline const std::string ToJson(T &instance, WriteConfig &&config)
{
    return ToJson(instance, config);
}

template <typename T>
inline const std::string ToJson(T &instance)
{
    return ToJson(instance, WriteConfig());
}

// // === Updating existing object, creating status === //

// // Update given object properties with a given string.
// // Example:
// //      User user;
// //      std::string json = "{}";
// //      ReadJson(json, user);
// // Returns a bound::ReadStatus instance.
// template <typename T>
// ReadStatus FromJson(const std::string &json, T &object)
// {
//     ReadStatus status;
//     bound::read::FromJson(json, object, status);
//     return status;
// }

// // Update given object properties with a given string.
// // Example:
// //      User user;
// //      ReadJson("{}", user);
// // Returns a bound::ReadStatus instance.
// template <typename T>
// inline ReadStatus FromJson(const std::string &&json, T &object)
// {
//     return FromJson(json, object);
// }

// // === Creating object, updating status === //

// // Create an object with a json string, and update passed read status.
// // Example:
// //      bound::ReadStatus status;
// //      User user = bound::FromJson(json);
// //      std::string json = "{}";
// //      ReadJson(json, user);
// // Returns a ReadStatus instance.
// template <typename T>
// T FromJson(const std::string &json, ReadStatus &status)
// {
//     T instance;
//     read::FromJson(json, instance, status);
//     return instance;
// }

// // Read from json string.
// // Example:
// //      User user;
// //      std::string json = "{}";
// //      ReadJson(json, user);
// // Returns a ReadStatus instance.
// template <typename T>
// inline T FromJson(const std::string &&json, ReadStatus &status)
// {
//     return FromJson<T>(json, status);
// }

// // === Creating object, unconcerned with status === //

// // Read from json string.
// // Example:
// //      User user;
// //      std::string json = "{}";
// //      ReadJson(json, user);
// // Returns a ReadStatus instance.
// template <typename T>
// T FromJson(const std::string &json)
// {
//     T instance;
//     ReadStatus status;
//     read::FromJson(json, instance, status);
//     return instance;
// }

// // Read from json string.
// // Example:
// //      User user;
// //      std::string json = "{}";
// //      ReadJson(json, user);
// // Returns a ReadStatus instance.
// template <typename T>
// inline T FromJson(const std::string &&json)
// {
//     return FromJson<T>(json);
// }

// // === ToJson === //

// // Read from json string.
// // Example:
// //      User user;
// //      std::string json = "{}";
// //      ReadJson(json, user);
// // Returns a ReadStatus instance.
// template <typename T>
// inline std::string ToJson(T &instance, WriteConfig &write_config)
// {
//     return write::ToJson(instance, write_config);
// }

// // Read from json string.
// // Example:
// //      User user;
// //      std::string json = "{}";
// //      ReadJson(json, user);
// // Returns a ReadStatus instance.
// template <typename T>
// inline std::string ToJson(T &instance, WriteConfig &&write_config)
// {
//     return ToJson(instance, write_config);
// }

// // Read from json string.
// // Example:
// //      User user;
// //      std::string json = "{}";
// //      ReadJson(json, user);
// // Returns a ReadStatus instance.
// template <typename T>
// inline std::string ToJson(T &instance)
// {
//     return ToJson(instance, WriteConfig());
// }

} // namespace bound

#endif