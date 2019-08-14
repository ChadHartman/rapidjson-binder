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

#ifndef BOUND_READ_PARSER_H_
#define BOUND_READ_PARSER_H_

#include <rapidjson/reader.h>
#include "event.h"

namespace bound
{

namespace read
{

// Parser which will tokenize JSON parse events
//  See bound.h for how to use
template <typename Stream>
class Parser
{
private:
    Event event_;
    rapidjson::Reader reader_;
    Stream stream_;

    bool is_reader_started_ = false;
    bool is_reader_complete_ = false;

public:
    Parser(Stream &&stream) : stream_{stream} {}

    const Event &event()
    {
        return event_;
    }

    // Returns true while there's new events
    bool FetchNextEvent()
    {
        if (!is_reader_started_)
        {
            reader_.IterativeParseInit();
            is_reader_started_ = true;
        }

        if (is_reader_complete_)
        {
            return false;
        }

        if (reader_.IterativeParseComplete())
        {
            is_reader_complete_ = true;
            event_.End();
            return false;
        }

        reader_.IterativeParseNext<rapidjson::kParseCommentsFlag>(stream_, event_);

        return true;
    }

    void Skip()
    {
        const Event::Type end_type = event_.type == Event::kTypeStartObject ? Event::kTypeEndObject : Event::kTypeEndArray;

        while (FetchNextEvent() && event_.type != end_type)
        {
            switch (event_.type)
            {
            case Event::kTypeStartObject:
            case Event::kTypeStartArray:
                Skip();
                break;
            }
        }
    }
};

} // namespace read

} // namespace bound

#endif