#ifndef BOUND_UTIL_TESTS_HPP_
#define BOUND_UTIL_TESTS_HPP_

#include "tests.h"

namespace util_tests
{

template <typename T>
struct Foo
{
    T getter()
    {
        T value;
        return value;
    }

    void setter(T value)
    {
    }

    void method()
    {
    }
};

TEST_CASE("Getter Setter Tests", "[util-is_getter]")
{
    using Type = int;
    using GetterType = decltype(&Foo<Type>::getter);
    using SetterType = decltype(&Foo<Type>::setter);
    using MethodType = decltype(&Foo<Type>::method);

    REQUIRE(bound::util::is_getter<GetterType>::value);
    REQUIRE(bound::util::is_setter<SetterType>::value);

    REQUIRE(!bound::util::is_setter<GetterType>::value);
    REQUIRE(!bound::util::is_getter<SetterType>::value);

    REQUIRE(!bound::util::is_setter<MethodType>::value);
    REQUIRE(!bound::util::is_getter<MethodType>::value);
}

TEST_CASE("Is Sequential Container Tests", "[util-is_seq_container]")
{
    REQUIRE(bound::util::is_seq_container<std::vector<int>>::value);
    REQUIRE(bound::util::is_seq_container<std::list<Foo<int>>>::value);
    REQUIRE(bound::util::is_seq_container<std::deque<std::string>>::value);

    REQUIRE(!bound::util::is_seq_container<std::vector<int> *>::value);
    REQUIRE(!bound::util::is_seq_container<Foo<int>>::value);
    REQUIRE(!bound::util::is_seq_container<int>::value);
}

TEST_CASE("Is Integer Tests", "[util-is_integer]")
{
    REQUIRE(bound::util::is_integer<int>::value);
    REQUIRE(bound::util::is_integer<const int>::value);
    REQUIRE(!bound::util::is_integer<unsigned>::value);
    REQUIRE(!bound::util::is_integer<bool>::value);
    REQUIRE(!bound::util::is_integer<double>::value);
}

TEST_CASE("Is Unsigned Integer Tests", "[util-is_unsigned_integer]")
{
    REQUIRE(!bound::util::is_unsigned_integer<int>::value);
    REQUIRE(bound::util::is_unsigned_integer<unsigned>::value);
    REQUIRE(bound::util::is_unsigned_integer<const unsigned>::value);
    REQUIRE(!bound::util::is_unsigned_integer<bool>::value);
    REQUIRE(!bound::util::is_unsigned_integer<double>::value);
}

TEST_CASE("Get Raw Type", "[util-raw_type]")
{
    using IType = bound::util::raw_type<const int>::type;
    IType i = 0;
    i = 1;

    using JType = bound::util::raw_type<const int &>::type;
    JType j = 0;
    j = 2;

    using KType = bound::util::raw_type<const int *>::type;
    KType k = 0;
    k = 1;
}

TEST_CASE("Is Simple Property", "[util-is_simple_property]")
{
    REQUIRE(bound::util::is_simple_property<bool>::value);
    REQUIRE(bound::util::is_simple_property<int>::value);
    REQUIRE(bound::util::is_simple_property<unsigned>::value);
    REQUIRE(bound::util::is_simple_property<float>::value);
    REQUIRE(bound::util::is_simple_property<const char *>::value);
    REQUIRE(bound::util::is_simple_property<std::string>::value);
    REQUIRE(!bound::util::is_simple_property<Foo<int>>::value);
    REQUIRE(!bound::util::is_simple_property<std::vector<int>>::value);
}

} // namespace util_tests

#endif
