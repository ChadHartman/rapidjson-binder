#ifndef BOUND_READER_READER_TESTS_H_
#define BOUND_READER_READER_TESTS_H_

#include "tests.h"

namespace reader_tests
{

void TestFoo(Foo &foo)
{
    REQUIRE("baz" == foo.bar().s);
    REQUIRE(3 == foo.bar().v.size());
    REQUIRE(1 == foo.bar().v[0]);
    REQUIRE(2 == foo.bar().v[1]);
    REQUIRE(3 == foo.bar().v[2]);
    REQUIRE(-100 == foo.bar().i);
}

TEST_CASE("Reader tests", "[reader]")
{
    bound::ReadStatus status;
    Foo foo;

    SECTION("Well formed")
    {
        std::string foo_json = "{\"bar\":{\"string\":\"baz\",\"array\":[1,2,3],\"integer\":-100}}";

        SECTION("Read object")
        {
            bound::reader::Parser<rapidjson::StringStream> parser{rapidjson::StringStream(foo_json.c_str())};
            bound::reader::Reader<rapidjson::StringStream>{parser}.Read(foo, status);
            if (!status.success())
            {
                printf("Failure: %s\n", status.error_message().c_str());
            }

            REQUIRE(status.success());
            TestFoo(foo);
        }

        SECTION("Read array")
        {
            std::vector<Foo> array;
            std::string array_json = "[" + foo_json + "]";
            bound::reader::Parser<rapidjson::StringStream> parser{rapidjson::StringStream(array_json.c_str())};
            bound::reader::Reader<rapidjson::StringStream>{parser}.Read(array, status);
            if (!status.success())
            {
                printf("Failure: %s\n", status.error_message().c_str());
            }
            REQUIRE(status.success());
            REQUIRE(1 == array.size());
            TestFoo(array[0]);
        }
    }

    SECTION("Malformed")
    {
        std::string foo_json = "{\"bar\"{\"string\":\"baz\",\"array\":[1,2,3],\"integer\":-100}}";

        SECTION("Read object")
        {
            bound::reader::Parser<rapidjson::StringStream> parser{rapidjson::StringStream(foo_json.c_str())};
            bound::reader::Reader<rapidjson::StringStream>{parser}.Read(foo, status);
            REQUIRE(false == status.success());
            REQUIRE(status.error_message().length() > 0);
        }

        SECTION("Read array")
        {
            std::vector<Foo> array;
            std::string array_json = "[" + foo_json + "]";
            bound::reader::Parser<rapidjson::StringStream> parser{rapidjson::StringStream(array_json.c_str())};
            bound::reader::Reader<rapidjson::StringStream>{parser}.Read(array, status);
            REQUIRE(false == status.success());
            REQUIRE(status.error_message().length() > 0);
        }
    }
}

} // namespace reader_tests

#endif