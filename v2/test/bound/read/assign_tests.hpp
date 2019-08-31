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