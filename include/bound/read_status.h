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

#ifndef BOUND_READ_STATUS_H_
#define BOUND_READ_STATUS_H_

#include <string>
#include "property.h"

namespace bound
{

// Result of a JSON Read operation
struct ReadStatus
{
private:
    std::string error_message_;

public:
    // Whether the read was successful
    bool success() const
    {
        return error_message_.length() == 0;
    };

    // The error message if one occurred
    std::string error_message() const
    {
        return error_message_;
    }

    // The error message setter
    void set_error_message(std::string error_message)
    {
        error_message_ = error_message;
    }

    std::string ToString() const
    {
        if (success())
        {
            return "{\"success\":true}";
        }

        return "{\"success\":false,\"error_message\":\"" +
               //  TODO: escape quotes
               error_message_ +
               "\"}";
    }
};

} // namespace bound

#endif