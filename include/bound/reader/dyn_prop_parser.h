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

#ifndef BOUND_READER_DYN_PROP_PARSER_H_
#define BOUND_READER_DYN_PROP_PARSER_H_

#include "rapidjson/writer.h"
#include "parser.h"
#include "../util.h"

namespace bound
{

namespace reader
{

// Parses out unbound keys to a DynamicProperties class
class DynPropParser
{
private:
    // Result of suggested operation
    enum LoopOperation
    {
        kLoopOpReturn,
        kLoopOpContinue,
        kLoopOpRecurse
    };

    // Writes  to the provided writer based on the given event
    LoopOperation UpdateWriter(const Event &event, rapidjson::Writer<rapidjson::StringBuffer> &writer)
    {
        switch (event.type)
        {
        case Event::kTypeNull:
            writer.Null();
            return kLoopOpContinue;

        case Event::kTypeBool:
            writer.Bool(event.value.bool_value);
            return kLoopOpContinue;

        case Event::kTypeInt:
            writer.Int(event.value.int_value);
            return kLoopOpContinue;

        case Event::kTypeUint:
            writer.Uint(event.value.unsigned_value);
            return kLoopOpContinue;

        case Event::kTypeInt64:
            writer.Int64(event.value.int64_t_value);
            return kLoopOpContinue;

        case Event::kTypeUint64:
            writer.Uint64(event.value.uint64_t_value);
            return kLoopOpContinue;

        case Event::kTypeDouble:
            writer.Double(event.value.double_value);
            return kLoopOpContinue;

        case Event::kTypeString:
            writer.String(event.string_value.c_str());
            return kLoopOpContinue;

        case Event::kTypeStartObject:
            writer.StartObject();
            return kLoopOpRecurse;

        case Event::kTypeKey:
            writer.Key(event.string_value.c_str());
            return kLoopOpContinue;

        case Event::kTypeStartArray:
            writer.StartArray();
            return kLoopOpRecurse;

        case Event::kTypeEndArray:
            writer.EndArray();
            return kLoopOpReturn;

        case Event::kTypeEndObject:
            writer.EndObject();
            return kLoopOpReturn;

        default:
            return kLoopOpReturn;
        }
    }

    // Write directly to string writer
    template <class Stream>
    void Traverse(Parser<Stream> &parser, rapidjson::Writer<rapidjson::StringBuffer> &writer)
    {
        while (parser.FetchNextEvent())
        {
            switch (UpdateWriter(parser.event(), writer))
            {
            case kLoopOpReturn:
                return;
            case kLoopOpContinue:
                break;
            case kLoopOpRecurse:
                // Recurse to stack on track with depth
                Traverse(parser, writer);
                break;
            }
        }
    }

    // Recusively skips unmapped sections of json
    template <typename Stream>
    void Skip(Parser<Stream> &parser)
    {
        if (parser.event().IsSimple())
        {
            // Skipped
            return;
        }

        const static unsigned long start_complex = Event::kTypeStartArray | Event::kTypeStartObject;
        const static unsigned long end_complex = Event::kTypeEndArray | Event::kTypeEndObject;

        while (parser.FetchNextEvent() && !(parser.event().type & end_complex))
        {
            if (parser.event().type & start_complex)
            {
                // Recurse lest we bail too early
                Skip(parser);
            }
        }
    }

    // Create a writer and recursively populate the final string with the unmapped json sections
    template <typename Stream>
    void UpdateDynamicProperties(DynamicProperties &json_props, std::string &key, Parser<Stream> &parser)
    {

        rapidjson::StringBuffer buffer;
        rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);

        UpdateWriter(parser.event(), writer);

        if (!parser.event().IsSimple())
        {
            Traverse(parser, writer);
        }

        std::string value{buffer.GetString()};
        json_props[key] = value;
    }

    // Needed for compilation, non-DynamicProperties are ignored
    template <typename T, typename Stream>
    typename std::enable_if_t<!std::is_same<T, DynamicProperties>::value>
    UpdateDynamicProperties(T &object, std::string &key, Parser<Stream> &parser) {}

    // Get the DynamicProperties reference from the object
    template <typename O, typename M, typename Stream>
    typename std::enable_if_t<std::is_member_object_pointer<M>::value>
    UpdateProperty(O &object, M member, std::string &key, Parser<Stream> &parser)
    {
        UpdateDynamicProperties(object.*(member), key, parser);
    }

    // Needed for compilation, methods are ignored
    template <typename O, typename M, typename Stream>
    typename std::enable_if_t<!std::is_member_object_pointer<M>::value>
    UpdateProperty(O &object, M member, std::string &key, Parser<Stream> &parser) {}

public:
    // Parses the unmapped section of json into a DynamicProperties collection, or recursively skips
    template <typename O, class Stream>
    void
    Parse(O &object, std::string &key, Parser<Stream> &parser)
    {
        constexpr auto prop_count = std::tuple_size<decltype(O::properties)>::value;
        bool found = false;

        util::for_sequence(std::make_index_sequence<prop_count>{}, [&](auto i) {
            constexpr auto property = std::get<i>(O::properties);

            if (property.is_dyn_props)
            {
                found = true;
                UpdateProperty(object, property.member, key, parser);
            }
        });

        if (!found)
        {
            Skip(parser);
        }
    }
};

} // namespace reader

} // namespace bound

#endif