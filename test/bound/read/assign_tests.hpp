#ifndef BOUND_READ_ASSIGN_TESTS_HPP_
#define BOUND_READ_ASSIGN_TESTS_HPP_

#include "tests.h"

namespace bound_read_assign_tests_hpp_
{

template <typename JsonType, typename ValueType>
void test(ValueType value, typename JsonType::type expected)
{
    JsonType target;

    REQUIRE(bound::read::is_assignable<JsonType, decltype(value)>::value);
    REQUIRE(!bound::read::is_unassignable<JsonType, decltype(value)>::value);

    REQUIRE(bound::read::Assign(target, value));
    REQUIRE(target.value == expected);
}

template <typename T>
void TestAssignNull(T initial, T expected)
{
    REQUIRE(bound::read::Assign(initial, nullptr));
    REQUIRE(initial == expected);
}

TEST_CASE("SFINAE", "[sfinae]")
{
    REQUIRE(bound::read::is_assignable<int &, int>::value);
    REQUIRE(bound::read::is_assignable<int &, const int &>::value);
    REQUIRE(bound::read::is_assignable<int &, long>::value);
    REQUIRE(bound::read::is_assignable<int &, const double &>::value);
    REQUIRE(bound::read::is_assignable<int &, bool &>::value);
    REQUIRE(bound::read::is_assignable<bool &, int>::value);
    REQUIRE(bound::read::is_assignable<double &, int>::value);
    REQUIRE(bound::read::is_assignable<bool &, double>::value);
}

TEST_CASE("Assign Tests", "[assign_tests]")
{
    SECTION("nulls")
    {
        TestAssignNull<bound::JsonString>({"default"}, {""});
        TestAssignNull<bound::JsonFloat>({22.22}, {0.0});
        TestAssignNull<bound::JsonBool>({true}, {false});
        TestAssignNull<bound::JsonUint>({111}, {0});
        TestAssignNull<bound::JsonInt>({-111}, {0});
        TestAssignNull<bound::JsonRaw>({"{\"message\":\"hello\"}"}, {""});
    }

    SECTION("JsonString|bool")
    {
        test<bound::JsonString, bool>(true, "true");
        test<bound::JsonString, bool>(false, "false");
    }
    SECTION("JsonString|int")
    {
        test<bound::JsonString, int>(-1, "-1");
        test<bound::JsonString, int>(42, "42");
    }
    SECTION("JsonString|unsigned")
    {
        test<bound::JsonString, unsigned>(0, "0");
        test<bound::JsonString, unsigned>(132, "132");
    }
    SECTION("JsonString|int64_t")
    {
        test<bound::JsonString, int64_t>(-239484, "-239484");
        test<bound::JsonString, int64_t>(239484, "239484");
    }
    SECTION("JsonString|uint64_t")
    {
        test<bound::JsonString, uint64_t>(0, "0");
        test<bound::JsonString, uint64_t>(203923049, "203923049");
    }
    SECTION("JsonString|string")
    {
        test<bound::JsonString, std::string>("foo", "foo");
        test<bound::JsonString, std::string>("bar", "bar");
    }
    SECTION("JsonString|double")
    {
        bound::JsonString target;
        double value = 0.0;

        REQUIRE(bound::read::is_assignable<bound::JsonString, decltype(value)>::value);
        REQUIRE(!bound::read::is_unassignable<bound::JsonString, decltype(value)>::value);

        REQUIRE(bound::read::Assign(target, value));
        REQUIRE(target.value.rfind("0.0", 0) == 0);

        value = 23429.1310941;
        REQUIRE(bound::read::Assign(target, value));
        REQUIRE(target.value.rfind("23429.131", 0) == 0);
    }

    SECTION("JsonFloat|bool")
    {
        test<bound::JsonFloat, bool>(true, 1.0);
        test<bound::JsonFloat, bool>(false, 0.0);
    }
    SECTION("JsonFloat|int")
    {
        test<bound::JsonFloat, int>(-1, -1.0);
        test<bound::JsonFloat, int>(42, 42.0);
    }
    SECTION("JsonFloat|unsigned")
    {
        test<bound::JsonFloat, unsigned>(0, 0.0);
        test<bound::JsonFloat, unsigned>(132, 132.0);
    }
    SECTION("JsonFloat|int64_t")
    {
        test<bound::JsonFloat, int64_t>(-239484, -239484.0);
        test<bound::JsonFloat, int64_t>(239484, 239484.0);
    }
    SECTION("JsonFloat|uint64_t")
    {
        test<bound::JsonFloat, uint64_t>(0, 0.0);
        test<bound::JsonFloat, uint64_t>(203923049, 203923049.0);
    }
    SECTION("JsonFloat|double")
    {
        test<bound::JsonFloat, double>(0.0, 0.0);
        test<bound::JsonFloat, double>(23429.1310941, 23429.1310941);
    }
    //--
    SECTION("JsonBool|bool")
    {
        test<bound::JsonBool, bool>(true, true);
        test<bound::JsonBool, bool>(false, false);
    }
    SECTION("JsonBool|int")
    {
        test<bound::JsonBool, int>(0, false);
        test<bound::JsonBool, int>(42, true);
    }
    SECTION("JsonBool|unsigned")
    {
        test<bound::JsonBool, unsigned>(0, false);
        test<bound::JsonBool, unsigned>(132, true);
    }
    SECTION("JsonFloat|int64_t")
    {
        test<bound::JsonBool, int64_t>(0, false);
        test<bound::JsonBool, int64_t>(239484, true);
    }
    SECTION("JsonFloat|uint64_t")
    {
        test<bound::JsonBool, uint64_t>(0, false);
        test<bound::JsonBool, uint64_t>(203923049, true);
    }
    SECTION("JsonFloat|double")
    {
        test<bound::JsonBool, double>(0.0, false);
        test<bound::JsonBool, double>(23429.1310941, true);
    }
    //---

    SECTION("JsonUint")
    {
        test<bound::JsonUint, bool>(true, 1);
        test<bound::JsonUint, bool>(false, 0);

        test<bound::JsonUint, int>(1, 1);
        test<bound::JsonUint, int>(42, 42);

        test<bound::JsonUint, unsigned>(0, 0);
        test<bound::JsonUint, unsigned>(132, 132);

        test<bound::JsonUint, int64_t>(1, 1);
        test<bound::JsonUint, int64_t>(239484, 239484);

        test<bound::JsonUint, uint64_t>(0, 0);
        test<bound::JsonUint, uint64_t>(203923049, 203923049);

        test<bound::JsonUint, double>(0.0, 0);
        test<bound::JsonUint, double>(23429.1310941, 23429);
    }
    //---

    SECTION("JsonInt")
    {
        test<bound::JsonInt, bool>(true, 1);
        test<bound::JsonInt, bool>(false, 0);

        test<bound::JsonInt, int>(-1, -1);
        test<bound::JsonInt, int>(42, 42);

        test<bound::JsonInt, unsigned>(0, 0);
        test<bound::JsonInt, unsigned>(132, 132);

        test<bound::JsonInt, int64_t>(-239484, -239484);
        test<bound::JsonInt, int64_t>(239484, 239484);

        test<bound::JsonInt, uint64_t>(0, 0);
        test<bound::JsonInt, uint64_t>(203923049, 203923049);

        test<bound::JsonInt, double>(0.0, 0);
        test<bound::JsonInt, double>(23429.1310941, 23429);
    }
}

} // namespace bound_read_assign_tests_hpp_

#endif