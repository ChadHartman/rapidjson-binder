#ifndef BOUND_READ_ASSIGN_TESTS_HPP_
#define BOUND_READ_ASSIGN_TESTS_HPP_

#include "tests.h"

namespace bound_read_assign_tests_hpp_
{

template <typename JsonType, typename ValueType>
void test(ValueType value, typename JsonType::type expected)
{
    bound::ReadStatus status;
    JsonType target;

    REQUIRE(bound::read::is_assignable<JsonType, decltype(value)>::value);
    REQUIRE(!bound::read::is_convertable<JsonType, decltype(value)>::value);
    REQUIRE(!bound::read::is_unassignable<JsonType, decltype(value)>::value);

    bound::read::Assign(target, value, status);
    REQUIRE(target.value == expected);
}

TEST_CASE("Assign Tests", "[assign_tests]")
{
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
    SECTION("JsonString|double")
    {
        // TODO test<bound::JsonString, double>(0.0, "0.000000");
        // TODO test<bound::JsonString, double>(23429.1310941, "23429.131");
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
    // SECTION("JsonUint")
    // {
    //     test<bound::JsonInt, bool>(true, 1);
    //     test<bound::JsonInt, bool>(false, 0);

    //     test<bound::JsonInt, int>(-1, -1);
    //     test<bound::JsonInt, int>(42, 42);

    //     test<bound::JsonInt, unsigned>(0, 0);
    //     test<bound::JsonInt, unsigned>(132, 132);

    //     test<bound::JsonInt, int64_t>(-239484, -239484);
    //     test<bound::JsonInt, int64_t>(239484, 239484);

    //     test<bound::JsonInt, uint64_t>(0, 0);
    //     test<bound::JsonInt, uint64_t>(203923049, 203923049);

    //     test<bound::JsonInt, double>(0.0, 0);
    //     test<bound::JsonInt, double>(23429.1310941, 23429);
    // }

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