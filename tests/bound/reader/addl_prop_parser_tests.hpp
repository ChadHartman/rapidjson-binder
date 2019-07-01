#ifndef BOUND_READER_ADDL_PROP_PARSER_TESTS_H_
#define BOUND_READER_ADDL_PROP_PARSER_TESTS_H_

#include "tests.h"

namespace addl_prop_parser_tests
{

TEST_CASE("Additional Property Parser tests", "[addl_prop_parser]")
{
    const std::string bar_input = "{"
                                  "\"string\":\"bar\","
                                  "\"array\":[-1,-2,-3],"
                                  "\"integer\":52,"
                                  "\"alpha\":{\"beta\":1},"
                                  "\"gamma\":[true],"
                                  "\"delta\":\"value\""
                                  "}";

    const std::string bar_output = "{"
                                   "\"string\":\"bar\","
                                   "\"array\":[-1,-2,-3],"
                                   "\"integer\":52"
                                   "}";

    const std::string foo_input = "{\"bar_property\":" + bar_input +
                                  ",\"bar\":" + bar_input +
                                  ",\"alpha\":{\"beta\":1},"
                                  "\"gamma\":[true],"
                                  "\"delta\":\"value\""
                                  "}";

    const std::string foo_output = "{\"bar_property\":" + bar_output +
                                   ",\"bar\":" + bar_output +
                                   ",\"alpha\":{\"beta\":1},"
                                   "\"delta\":\"value\","
                                   "\"gamma\":[true]"
                                   "}";

    Foo foo;
    bound::ReadStatus status = bound::FromJson(foo_input, foo);
    if (!status.success())
    {
        printf("Failure: %s\n", status.error_message().c_str());
    }

    std::string out = bound::ToJson(foo);
    REQUIRE(out == foo_output);
}

} // namespace addl_prop_parser_tests

#endif