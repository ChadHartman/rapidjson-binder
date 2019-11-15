#ifndef BOUND_READ_RAW_JSON_READER_TESTS_HPP_
#define BOUND_READ_RAW_JSON_READER_TESTS_HPP_

#include "tests.h"

namespace bound_read_raw_json_reader_tests_hpp_
{

void TestReadRawJson(std::string &&json)
{
    bound::JsonRaw raw_json;
    bound::read::Parser<rapidjson::StringStream> parser{rapidjson::StringStream(json.c_str())};
    bound::read::RawJsonReader<rapidjson::StringStream>(parser).Read(raw_json);
    REQUIRE(json == raw_json.value);
}

TEST_CASE("Raw Json Reader Tests", "[raw_json_reader_tests]")
{

    SECTION("Null")
    {
        TestReadRawJson("null");
    }

    SECTION("Bool")
    {
        TestReadRawJson("true");
    }
    SECTION("Int")
    {
        TestReadRawJson("-3200");
    }
    SECTION("Uint")
    {
        TestReadRawJson("3200");
    }
    SECTION("Double")
    {
        TestReadRawJson("44.44");
    }

    SECTION("String")
    {
        TestReadRawJson("\"Foo\"");
    }

    SECTION("Object")
    {
        TestReadRawJson("{\"Foo\":\"Bar\"}");
    }

    SECTION("Array")
    {
        TestReadRawJson("[\"Foo\",\"Bar\"]");
    }

    SECTION("Object Recurse")
    {
        TestReadRawJson("{\"Foo\":{\"Bar\":\"Baz\"}}");
    }

    SECTION("Array Recurse")
    {
        TestReadRawJson("[[\"Foo\"],[\"Bar\"]]");
    }

    SECTION("Object Mix")
    {
        TestReadRawJson("{\"Foo\":[\"Bar\",\"Baz\"]}");
    }

    SECTION("Array Mix")
    {
        TestReadRawJson("[{\"Foo\":\"Bar\"}]");
    }
}
} // namespace bound_read_raw_json_reader_tests_hpp_

#endif