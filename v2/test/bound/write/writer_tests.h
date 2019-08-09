#ifndef BOUND_WRITE_WRITER_TESTS_H_
#define BOUND_WRITE_WRITER_TESTS_H_

#include "../../tests.h"

namespace bound_write_writer_tests_h_
{

template <typename T>
class Foo
{
private:
    T alpha_;
    T *gamma_;

public:
    T beta;

    Foo(T alpha, T beta)
        : alpha_{alpha}, beta{beta} {}

    T alpha()
    {
        return alpha_;
    }

    T *gamma()
    {
        return &alpha_;
    }

    T *gamma_null()
    {
        return NULL;
    }

    T &delta()
    {
        return alpha_;
    }

    const T &epsilon()
    {
        return alpha_;
    }

    const T &zeta() const
    {
        return alpha_;
    }

    constexpr static auto BOUND_PROPS_NAME = std::make_tuple(
        bound::property(&Foo::alpha, "alpha"),
        bound::property(&Foo::beta, "beta"),
        bound::property(&Foo::gamma, "gamma"),
        bound::property(&Foo::gamma_null, "gamma_null"),
        bound::property(&Foo::delta, "delta"),
        bound::property(&Foo::epsilon, "epsilon"),
        bound::property(&Foo::epsilon, "zeta"));
};

template <typename T>
struct Child
{
    T value;

    constexpr static auto BOUND_PROPS_NAME = std::make_tuple(
        bound::property(&Child::value, "value"));
};

template <typename T>
struct Convertible
{
    T value;

    operator T()
    {
        return value;
    }
};

struct ConstChild
{
    std::string value() const
    {
        return "value";
    }

    constexpr static auto BOUND_PROPS_NAME = std::make_tuple(
        bound::property(&ConstChild::value, "value"));
};

TEST_CASE("Writer Tests", "[writer_tests]")
{
    SECTION("String Tests")
    {
        Foo<std::string> foo{"hello", "world"};
        REQUIRE(
            "{\"alpha\":\"hello\",\"beta\":\"world\",\"gamma\":\"hello\",\"gamma_null\":null,\"delta\":\"hello\",\"epsilon\":\"hello\",\"zeta\":\"hello\"}" ==
            bound::write::ToJson(foo));
    }

    SECTION("bool Tests")
    {
        Foo<bool> foo{true, false};
        REQUIRE(
            "{\"alpha\":true,\"beta\":false,\"gamma\":true,\"gamma_null\":null,\"delta\":true,\"epsilon\":true,\"zeta\":true}" ==
            bound::write::ToJson(foo));
    }

    SECTION("Integer Tests")
    {
        Foo<int> foo{-4, 5};
        REQUIRE(
            "{\"alpha\":-4,\"beta\":5,\"gamma\":-4,\"gamma_null\":null,\"delta\":-4,\"epsilon\":-4,\"zeta\":-4}" ==
            bound::write::ToJson(foo));
    }

    SECTION("Uinteger Tests")
    {
        Foo<unsigned> foo{4, 5};
        REQUIRE(
            "{\"alpha\":4,\"beta\":5,\"gamma\":4,\"gamma_null\":null,\"delta\":4,\"epsilon\":4,\"zeta\":4}" ==
            bound::write::ToJson(foo));
    }

    SECTION("Float Tests")
    {
        Foo<double> foo{4.4, 5.5};
        REQUIRE(
            "{\"alpha\":4.4,\"beta\":5.5,\"gamma\":4.4,\"gamma_null\":null,\"delta\":4.4,\"epsilon\":4.4,\"zeta\":4.4}" ==
            bound::write::ToJson(foo));
    }

    SECTION("Child<string> Tests")
    {
        Foo<Child<std::string>> foo{{"alpha"}, {"beta"}};
        REQUIRE(
            "{\"alpha\":{\"value\":\"alpha\"},\"beta\":{\"value\":\"beta\"},\"gamma\":{\"value\":\"alpha\"},\"gamma_null\":null,\"delta\":{\"value\":\"alpha\"},\"epsilon\":{\"value\":\"alpha\"},\"zeta\":{\"value\":\"alpha\"}}" ==
            bound::write::ToJson(foo));
    }

    SECTION("Child<int> Tests")
    {
        Foo<Child<int>> foo{{-1}, {2}};
        REQUIRE(
            "{\"alpha\":{\"value\":-1},\"beta\":{\"value\":2},\"gamma\":{\"value\":-1},\"gamma_null\":null,\"delta\":{\"value\":-1},\"epsilon\":{\"value\":-1},\"zeta\":{\"value\":-1}}" ==
            bound::write::ToJson(foo));
    }

    SECTION("const Child<int> Tests")
    {
        const ConstChild const_child;
        REQUIRE("{\"value\":\"value\"}" == bound::write::ToJson(const_child));
    }

    SECTION("Foo<Array<string>> Tests")
    {
        Foo<std::vector<std::string>> foo{{"alpha", "beta"}, {"gamma", "delta"}};
        REQUIRE("{\"alpha\":[\"alpha\",\"beta\"],\"beta\":[\"gamma\",\"delta\"],\"gamma\":[\"alpha\",\"beta\"],"
                "\"gamma_null\":null,\"delta\":[\"alpha\",\"beta\"],\"epsilon\":[\"alpha\",\"beta\"],\"zeta\":"
                "[\"alpha\",\"beta\"]}" == bound::write::ToJson(foo));
    }

    SECTION("Foo<Array<int>> Tests")
    {
        Foo<std::vector<int>> foo = {{-1, 2}, {-3, 4}};
        REQUIRE(
            "{\"alpha\":[-1,2],\"beta\":[-3,4],\"gamma\":[-1,2],\"gamma_null\":null,\"delta\":[-1,2],\"epsilon\":[-1,2],\"zeta\":[-1,2]}" ==
            bound::write::ToJson(foo));
    }

    SECTION("Array<string> Tests")
    {
        std::vector<std::string> array = {"alpha", "beta"};
        REQUIRE("[\"alpha\",\"beta\"]" == bound::write::ToJson(array));
    }

    SECTION("Array<int> Tests")
    {
        std::vector<int> array = {-1, 2};
        REQUIRE("[-1,2]" == bound::write::ToJson(array));
    }

    SECTION("JsonRaw Tests")
    {
        Convertible<bound::JsonRaw> convertible;
        convertible.value.value = "{\"dynamic-key\":true}";
        REQUIRE(convertible.value.value == bound::write::ToJson(convertible));
    }

    SECTION("JsonString Tests")
    {
        Convertible<bound::JsonString> convertible;
        convertible.value.value = "Some string value with \"quotes\", and \"{\"";
        REQUIRE("\"Some string value with \\\"quotes\\\", and \\\"{\\\"\"" == bound::write::ToJson(convertible));
    }

    SECTION("JsonDouble Tests")
    {
        Convertible<bound::JsonDouble> convertible;
        convertible.value.value = 33.33;
        REQUIRE("33.33" == bound::write::ToJson(convertible));
    }

    SECTION("JsonBool Tests")
    {
        Convertible<bound::JsonBool> convertible;
        convertible.value.value = true;
        REQUIRE("true" == bound::write::ToJson(convertible));
    }

    SECTION("JsonInt Tests")
    {
        Convertible<bound::JsonInt> convertible;
        convertible.value.value = -1000;
        REQUIRE("-1000" == bound::write::ToJson(convertible));
    }

    SECTION("JsonUint Tests")
    {
        Convertible<bound::JsonUint> convertible;
        convertible.value.value = 255;
        REQUIRE("255" == bound::write::ToJson(convertible));
    }
}

} // namespace bound_write_writer_tests_h_

#endif