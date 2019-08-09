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
struct Bar
{
    T value;

    operator T()
    {
        return value;
    }
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

    SECTION("JsonRaw Tests")
    {
        Bar<bound::JsonRaw> bar;
        bar.value.value = "{\"dynamic-key\":true}";
        REQUIRE(bar.value.value == bound::write::ToJson(bar));
    }

    SECTION("JsonString Tests")
    {
        Bar<bound::JsonString> bar;
        bar.value.value = "Some string value with \"quotes\", and \"{\"";
        REQUIRE("\"Some string value with \\\"quotes\\\", and \\\"{\\\"\"" == bound::write::ToJson(bar));
    }

    SECTION("JsonDouble Tests")
    {
        Bar<bound::JsonDouble> bar;
        bar.value.value = 33.33;
        REQUIRE("33.33" == bound::write::ToJson(bar));
    }

    SECTION("JsonBool Tests")
    {
        Bar<bound::JsonBool> bar;
        bar.value.value = true;
        REQUIRE("true" == bound::write::ToJson(bar));
    }

    SECTION("JsonInt Tests")
    {
        Bar<bound::JsonInt> bar;
        bar.value.value = -1000;
        REQUIRE("-1000" == bound::write::ToJson(bar));
    }

    SECTION("JsonUint Tests")
    {
        Bar<bound::JsonUint> bar;
        bar.value.value = 255;
        REQUIRE("255" == bound::write::ToJson(bar));
    }
}

} // namespace bound_write_writer_tests_h_

#endif