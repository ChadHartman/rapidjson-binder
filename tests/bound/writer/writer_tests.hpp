#ifndef BOUND_WRITER_WRITER_TESTS_H_
#define BOUND_WRITER_WRITER_TESTS_H_

#include "tests.h"

namespace writer_tests
{

TEST_CASE("Write tests", "[writer]")
{
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> rapidjson_writer(buffer);

    bound::WriteConfig write_config;
    bound::writer::Writer<rapidjson::StringBuffer> writer(rapidjson_writer, write_config);

    SECTION("Unfiltered")
    {
        // Simple properties
        SECTION("Write bool")
        {
            writer.Write(false);
            std::string value{buffer.GetString()};
            REQUIRE("false" == value);
        }

        SECTION("Write unsigned")
        {
            writer.Write(0u);
            std::string value{buffer.GetString()};
            REQUIRE("0" == value);
        }

        SECTION("Write int")
        {
            writer.Write(0);
            std::string value{buffer.GetString()};
            REQUIRE("0" == value);
        }

        SECTION("Write float")
        {
            writer.Write(0.0);
            std::string value{buffer.GetString()};
            REQUIRE("0.0" == value);
        }

        SECTION("Write c-string")
        {
            writer.Write("foo");
            std::string value{buffer.GetString()};
            REQUIRE("\"foo\"" == value);
        }

        SECTION("Write string")
        {
            std::string s{""};
            writer.Write(s);
            std::string value{buffer.GetString()};
            REQUIRE("\"\"" == value);
        }

        SECTION("Write array primative")
        {
            std::vector<int> array = {-1, 0, 1};
            writer.Write(array);
            std::string value{buffer.GetString()};
            REQUIRE("[-1,0,1]" == value);
        }

        SECTION("Write array string")
        {
            std::vector<std::string> array = {"alpha", "beta", "gamma"};
            writer.Write(array);
            std::string value{buffer.GetString()};
            REQUIRE("[\"alpha\",\"beta\",\"gamma\"]" == value);
        }

        SECTION("Write object")
        {
            Foo foo;
            writer.Write(foo);
            std::string value{buffer.GetString()};
            REQUIRE("{\"bar_property\":{\"string\":\"\",\"array\":[],\"integer\":0},\"bar\":{\"string\":\"\",\"array\":[],\"integer\":0}}" == value);
        }
    }

    SECTION("Filtered")
    {
        write_config.Filter(false);
        write_config.FilterEmptiesAndZeroes();

        SECTION("Write array primative")
        {
            std::vector<int> array = {-1, 0, 1};
            writer.Write(array);
            std::string value{buffer.GetString()};
            REQUIRE("[-1,1]" == value);
        }

        SECTION("Write array string")
        {
            std::vector<std::string> array = {"alpha", "", "gamma"};
            writer.Write(array);
            std::string value{buffer.GetString()};
            REQUIRE("[\"alpha\",\"gamma\"]" == value);
        }

        SECTION("Write object")
        {
            Foo foo;
            writer.Write(foo);
            std::string value{buffer.GetString()};
            REQUIRE("{}" == value);
        }
    }
}

} // namespace writer_tests

#endif