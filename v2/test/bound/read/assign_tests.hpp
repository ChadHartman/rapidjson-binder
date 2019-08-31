#ifndef BOUND_READ_ASSIGN_TESTS_HPP_
#define BOUND_READ_ASSIGN_TESTS_HPP_

#include "tests.h"

namespace bound_read_assign_tests_hpp_
{

template <typename JsonType, typename ValueType, typename InterpretedType>
void test(ValueType value, InterpretedType expected)
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
    // bound::JsonInt json_int;
    // int64_t value = 12;
    bound::ReadStatus status;
    // bound::read::Assign(json_int, value, status);

    // using A = bound::JsonInt;
    // using B = bool &;

    SECTION("JsonInt|bool")
    {
        test<bound::JsonInt, int64_t, bool>(false, 0);
        test<bound::JsonInt, int64_t, bool>(true, 1);
    }
}

} // namespace bound_read_assign_tests_hpp_

#endif