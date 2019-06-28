#ifndef BOUND_READER_PARSER_TESTS_H_
#define BOUND_READER_PARSER_TESTS_H_

#include "tests.h"

namespace parser_tests
{

TEST_CASE("Parser tests", "[parser]")
{
    const char *json = "{\"alpha\":\"beta\",\"gamma\":{\"delta\":0},\"epsilon\":[true]}";
    const int token_count = 13;
    bound::reader::Parser<rapidjson::StringStream> parser{rapidjson::StringStream(json)};
    REQUIRE(bound::reader::Event::kTypeBegin == parser.event().type);

    int count = 0;
    for (; parser.FetchNextEvent(); ++count)
    {
    }

    REQUIRE(token_count == count);
    REQUIRE(bound::reader::Event::kTypeEnd == parser.event().type);
}

} // namespace parser_tests

#endif