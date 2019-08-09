#ifndef TEST_INCLUDE_BOUND_TESTS_H_
#define TEST_INCLUDE_BOUND_TESTS_H_

namespace test_include_bound_tests_h_
{

struct Foo
{
};

TEST_CASE("Bound FromJson Tests", "[bound-from_json]")
{
    Foo foo;
    bound::FromJson("{}", foo);
}

TEST_CASE("Bound ToJson Tests", "[bound-to_json]")
{
    Foo foo;
    printf("%s\n", bound::ToJson(foo).c_str());
}

} // namespace test_include_bound_tests_h_

#endif