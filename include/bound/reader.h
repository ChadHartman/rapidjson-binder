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

#ifndef BOUND_READER_H_
#define BOUND_READER_H_

#include <map>

#include "parser.h"
#include "read_status.h"
#include "util.h"

namespace bound
{

namespace reader
{

// Map for stringifying event types
const std::map<Event::Type, std::string> kEventTypeNames{
    {Event::kTypeBegin, "Begin"},
    {Event::kTypeNull, "Null"},
    {Event::kTypeBool, "Bool"},
    {Event::kTypeInt, "Int"},
    {Event::kTypeUint, "Uint"},
    {Event::kTypeInt64, "Int64"},
    {Event::kTypeUint64, "Uint64"},
    {Event::kTypeDouble, "Double"},
    {Event::kTypeString, "String"},
    {Event::kTypeStartObject, "StartObject"},
    {Event::kTypeKey, "Key"},
    {Event::kTypeEndObject, "EndObject"},
    {Event::kTypeStartArray, "StartArray"},
    {Event::kTypeEndArray, "EndArray"},
    {Event::kTypeEnd, "End"}};

const static long kEventTypeSimple =
    Event::kTypeBool |
    Event::kTypeInt |
    Event::kTypeUint |
    Event::kTypeInt64 |
    Event::kTypeUint64 |
    Event::kTypeDouble |
    Event::kTypeString;

const static long kEventTypeStartComplex = Event::kTypeStartObject | Event::kTypeStartArray;
const static long kEventTypeEndOfDocument = Event::kTypeEnd | Event::kTypeEndObject;
const static long kEventTypeStartProperty = kEventTypeSimple | kEventTypeStartComplex;

// Getter reader
//  Getters are unused for deserialization
//  Returns false as all executions are unsuccessful method calls
template <class Stream, typename O, class Class, typename T>
typename std::enable_if<util::is_getter<T Class::*>::value, bool>::type
ProcessMember(Parser<Stream> &parser, O &object, T Class::*member, ReadStatus &status)
{
    return false;
}

// Setter reader
//  Creates instance of target value, and passes that as a reference,
//      which is ultimately passed into the setter
//  Returns status of read
template <class Stream, typename O, class Class, typename T>
typename std::enable_if<util::is_setter<T Class::*>::value, bool>::type
ProcessMember(Parser<Stream> &parser, O &object, T Class::*member, ReadStatus &status)
{
    // Setters are always guaranteed to have a single param,
    //  remove reference if is exists so we can instantiate an
    //  instance to write to
    using Type = typename std::remove_reference<
        typename util::function_traits<decltype(member)>::template arg<0>::type>::type;
    Type value;
    bool success = false;

    if (parser.event().type() & kEventTypeStartComplex)
    {
        // Token is "{" or "["
        Read(parser, value, status);
        success = status.success();
    }
    else
    {
        success = parser.event().SetValue(value);
    }

    // Call the setter with the read value
    (object.*(member))(value);

    return success;
}

// Field reader
//  Writes to the reference
//  Returns status of read
template <class Stream, typename O, class Class, typename T>
typename std::enable_if<!std::is_member_function_pointer<T Class::*>::value, bool>::type
ProcessMember(Parser<Stream> &parser, O &object, T Class::*member, ReadStatus &status)
{
    // Going to read into the address directly
    T &child = object.*(member);

    if (parser.event().type() & kEventTypeStartComplex)
    {
        // Token is "{" or "["
        Read(parser, child, status);
        return status.success();
    }

    return parser.event().SetValue(child);
}

// Set an object property of specified key
template <class Stream, class T>
void SetProperty(Parser<Stream> &parser, T &instance, std::string &key, ReadStatus &status)
{
    constexpr auto prop_count = std::tuple_size<decltype(T::properties)>::value;
    bool found = false;

    util::for_sequence(std::make_index_sequence<prop_count>{}, [&](auto i) {
        constexpr auto property = std::get<i>(T::properties);
        if (property.name != key || util::is_getter<decltype(property.member)>::value)
        {
            // Ignoring wrong keys and getters
            return;
        }

        found = true;

        if (ProcessMember(parser, instance, property.member, status))
        {
            return;
        }

        // Error has occurred
        if (status.success())
        {
            // Shallow property error occurred (string, int, bool, double...).
            status.set_error_message(
                "Error setting value for key \"" + key +
                "\"; object property is not compatible with event " + kEventTypeNames.at(parser.event().type()) +
                ".");
            return;
        }

        // Complete property error occurred (recursive), provide key info with prior error.
        status.set_error_message(
            "Error setting value for key \"" + key +
            "\"; " + status.error_message());
    });

    if (found)
    {
        return;
    }

    status.set_error_message("No property with name \"" + key + "\" was found.");
}

// Read from simple property; should be unreachable, needs to exist to compile
template <class Stream, class T>
typename std::enable_if<std::is_fundamental<T>::value || std::is_same<T, std::string>::value>::type
Read(Parser<Stream> &parser, T &instance, ReadStatus &status)
{
    status.set_error_message(
        "Attempted recursive read of " + kEventTypeNames.at(parser.event().type()) + " event.");
}

// Read into vector property
template <class Stream, template <typename...> class Container, typename T>
typename std::enable_if<util::is_seq_container<Container<T>>::value>::type
Read(Parser<Stream> &parser, Container<T> &container, ReadStatus &status)
{
    if (parser.event().type() != Event::kTypeStartArray)
    {
        status.set_error_message(
            "Expected " + kEventTypeNames.at(Event::kTypeStartArray) +
            " but was " + kEventTypeNames.at(parser.event().type()) + ".");
        return;
    }

    // Reset for clean slate
    container.clear();

    while (parser.FetchNextEvent())
    {
        switch (parser.event().type())
        {
        case Event::kTypeEndArray:
            // Complete
            return;
        case Event::kTypeNull:
            // Do nothing
            break;
        case Event::kTypeBool:
        case Event::kTypeInt:
        case Event::kTypeUint:
        case Event::kTypeInt64:
        case Event::kTypeUint64:
        case Event::kTypeDouble:
        case Event::kTypeString:
        {
            T value;
            if (!parser.event().SetValue(value))
            {
                status.set_error_message(
                    "Unable to add element " + kEventTypeNames.at(parser.event().type()) +
                    " to array.");
                return;
            }
            container.push_back(value);
            break;
        }
        case Event::kTypeStartObject:
        case Event::kTypeStartArray:
        {
            T child;
            Read(parser, child, status);
            if (!status.success())
            {
                // Error has occurred
                return;
            }
            container.push_back(child);
            break;
        }
        default:
            status.set_error_message(
                "Unexpected event: " + kEventTypeNames.at(parser.event().type()) + ".");
            return;
        }
    }

    // Should never happen.
    status.set_error_message(
        "Unreachable section reached for event: " +
        kEventTypeNames.at(parser.event().type()) + ".");
}

// Read into object
template <class Stream, class T>
typename std::enable_if<!std::is_fundamental<T>::value && !std::is_same<T, std::string>::value>::type
Read(Parser<Stream> &parser, T &instance, ReadStatus &status)
{
    std::string key;
    bool last_event_was_key = false;

    // Start of document
    if (parser.event().type() == Event::kTypeBegin)
    {
        parser.FetchNextEvent();
    }

    // Verify initial
    if (parser.event().type() != Event::kTypeStartObject)
    {
        status.set_error_message(
            "Expected start of object, but was " +
            kEventTypeNames.at(parser.event().type()) +
            ".");
        return;
    }

    while (parser.FetchNextEvent())
    {
        switch (parser.event().type())
        {
        case Event::kTypeEnd:
        case Event::kTypeEndObject:
            // Completed
            return;
        case Event::kTypeNull:
            // Do nothing will null values
            if (!last_event_was_key)
            {
                status.set_error_message(
                    "No key immediately preceeded parsed null, last key parsed was \"" + key + "\".");
                return;
            }
            last_event_was_key = false;
            // Do nothing
            break;
        case Event::kTypeKey:
            if (last_event_was_key)
            {
                status.set_error_message(
                    "Key \"" + key + "\" was already found but not set.");
                return;
            }

            if (!parser.event().SetValue(key))
            {
                status.set_error_message(
                    "Unexpected error occurred retrieving key, last key was \"" + key + "\".");
                return;
            }

            last_event_was_key = true;
            break;

        case Event::kTypeBool:
        case Event::kTypeInt:
        case Event::kTypeUint:
        case Event::kTypeInt64:
        case Event::kTypeUint64:
        case Event::kTypeDouble:
        case Event::kTypeString:
        case Event::kTypeStartObject:
        case Event::kTypeStartArray:
            if (!last_event_was_key)
            {
                status.set_error_message(
                    "No key preceeded event " + kEventTypeNames.at(parser.event().type()) +
                    ". Last key parsed was \"" + key + "\".");
                return;
            }

            last_event_was_key = false;

            SetProperty(parser, instance, key, status);
            if (!status.success())
            {
                // Bail early on error
                return;
            }
            break;
        default:
            // All cases covered in well-formed JSON, error must have occurred
            status.set_error_message(
                "Unexpected event: " + kEventTypeNames.at(parser.event().type()) +
                ". Last key was \"" + key + "\".");

            return;
        }
    }
    // Should never happen.
    status.set_error_message(
        "Unreachable section reached for event: " + kEventTypeNames.at(parser.event().type()) +
        ". Last key was \"" + key + "\".");
}

} // namespace reader

} // namespace bound

#endif