#ifndef TEST_INCLUDE_BOUND_TESTS_H_
#define TEST_INCLUDE_BOUND_TESTS_H_

namespace test_include_bound_tests_h_
{

struct Foo
{
};

template <typename T>
typename std::enable_if_t<std::is_convertible<T, long long>::value>
Write(T value)
{
    long long v = (long long)value;

    printf("%lld\n", v);
}

template <typename T>
typename std::enable_if_t<std::is_convertible<T, std::string>::value>
Write(T value)
{
    std::string s = (std::string)value;

    printf("%s\n", s.c_str());
}

TEST_CASE("Bound FromJson Tests", "[bound-from_json]")
{
    // Foo foo;
    //bound::FromJson("{}", foo);
}

TEST_CASE("Bound ToJson Tests", "[bound-to_json]")
{
    // Foo foo;
    //   printf("%s\n", bound::ToJson(foo).c_str());
    Write("Hello");
    Write(56);
}

} // namespace test_include_bound_tests_h_

#endif