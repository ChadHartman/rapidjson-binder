#ifndef BOUND_CONST_CHAR_POINTER_TESTS_HPP_
#define BOUND_CONST_CHAR_POINTER_TESTS_HPP_

#include "tests.h"

namespace prop_class_tests
{

TEST_CASE("bool tests", "[bool]")
{
    test::PropClass<bool> object;
    std::string json{"{\"alpha\":true,\"beta\":false,\"gamma\":true,\"delta\":false}"};
    bound::ReadStatus status = bound::FromJson(json, object);

    if (!status.success())
    {
        printf("Failure: %s\n", status.error_message().c_str());
    }

    REQUIRE(status.success());

    REQUIRE(object.alpha);

    REQUIRE(!object.beta());
    REQUIRE(object.gamma());
    REQUIRE(!*object.delta());

    REQUIRE(json == bound::ToJson(object));
}

TEST_CASE("int tests", "[int]")
{
    test::PropClass<int> object;
    std::string json{"{\"alpha\":-1,\"beta\":-2,\"gamma\":-3,\"delta\":-4}"};
    bound::ReadStatus status = bound::FromJson(json, object);

    if (!status.success())
    {
        printf("Failure: %s\n", status.error_message().c_str());
    }

    REQUIRE(status.success());

    REQUIRE(-1 == object.alpha);

    REQUIRE(-2 == object.beta());
    REQUIRE(-3 == object.gamma());
    REQUIRE(-4 == *object.delta());

    REQUIRE(json == bound::ToJson(object));
}

TEST_CASE("unsigned tests", "[unsigned]")
{
    test::PropClass<unsigned> object;
    std::string json{"{\"alpha\":1,\"beta\":2,\"gamma\":3,\"delta\":4}"};
    bound::ReadStatus status = bound::FromJson(json, object);

    if (!status.success())
    {
        printf("Failure: %s\n", status.error_message().c_str());
    }

    REQUIRE(status.success());

    REQUIRE(1 == object.alpha);

    REQUIRE(2 == object.beta());
    REQUIRE(3 == object.gamma());
    REQUIRE(4 == *object.delta());

    REQUIRE(json == bound::ToJson(object));
}

TEST_CASE("double tests", "[double]")
{
    test::PropClass<double> object;
    std::string json{"{\"alpha\":1.1,\"beta\":2.2,\"gamma\":3.3,\"delta\":4.4}"};
    bound::ReadStatus status = bound::FromJson(json, object);

    if (!status.success())
    {
        printf("Failure: %s\n", status.error_message().c_str());
    }

    REQUIRE(status.success());

    REQUIRE(1.1 == object.alpha);

    REQUIRE(2.2 == object.beta());
    REQUIRE(3.3 == object.gamma());
    REQUIRE(4.4 == *object.delta());

    REQUIRE(json == bound::ToJson(object));
}

TEST_CASE("std::string tests", "[std_string]")
{
    test::PropClass<std::string> object;
    std::string json{"{\"alpha\":\"123 Fake St\",\"beta\":\"John\",\"gamma\":\"Mr\",\"delta\":\"delta\"}"};
    bound::ReadStatus status = bound::FromJson(json, object);

    if (!status.success())
    {
        printf("Failure: %s\n", status.error_message().c_str());
    }

    REQUIRE(status.success());

    REQUIRE("123 Fake St" == object.alpha);
    REQUIRE("John" == object.beta());
    REQUIRE("Mr" == object.gamma());
    REQUIRE("delta" == *object.delta());

    REQUIRE(json == bound::ToJson(object));
}

TEST_CASE("primative array tests", "[array_of_primatives]")
{
    test::PropClass<std::vector<int>> object;
    std::string json{"{\"alpha\":[3],\"beta\":[4],\"gamma\":[5],\"delta\":[6]}"};
    bound::ReadStatus status = bound::FromJson(json, object);

    if (!status.success())
    {
        printf("Failure: %s\n", status.error_message().c_str());
    }

    REQUIRE(status.success());

    REQUIRE(3 == object.alpha[0]);

    REQUIRE(4 == object.beta()[0]);
    REQUIRE(5 == object.gamma()[0]);
    REQUIRE(6 == (*object.delta())[0]);

    REQUIRE(json == bound::ToJson(object));
}

struct Child
{
    unsigned age;
    constexpr static auto properties = std::make_tuple(
        bound::property(&Child::age, "age"));
};

TEST_CASE("object tests", "[object]")
{
    test::PropClass<Child> object;
    std::string json{"{\"alpha\":{\"age\":3},\"beta\":{\"age\":4},\"gamma\":{\"age\":5},\"delta\":{\"age\":6}}"};
    bound::ReadStatus status = bound::FromJson(json, object);

    if (!status.success())
    {
        printf("Failure: %s\n", status.error_message().c_str());
    }

    REQUIRE(status.success());

    REQUIRE(3 == object.alpha.age);

    REQUIRE(4 == object.beta().age);
    REQUIRE(5 == object.gamma().age);
    REQUIRE(6 == object.delta()->age);

    REQUIRE(json == bound::ToJson(object));
}

TEST_CASE("object array tests", "[array_of_objects]")
{
    test::PropClass<std::vector<Child>> object;
    std::string json{"{\"alpha\":[{\"age\":3}],\"beta\":[{\"age\":4}],\"gamma\":[{\"age\":5}],\"delta\":[{\"age\":6}]}"};
    bound::ReadStatus status = bound::FromJson(json, object);

    if (!status.success())
    {
        printf("Failure: %s\n", status.error_message().c_str());
    }

    REQUIRE(status.success());

    REQUIRE(3 == object.alpha[0].age);

    REQUIRE(4 == object.beta()[0].age);
    REQUIRE(5 == object.gamma()[0].age);
    REQUIRE(6 == (*object.delta())[0].age);

    REQUIRE(json == bound::ToJson(object));
}

} // namespace prop_class_tests

#endif