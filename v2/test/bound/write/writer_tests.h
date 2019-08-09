#ifndef BOUND_WRITE_WRITER_TESTS_H_
#define BOUND_WRITE_WRITER_TESTS_H_

#include "../../tests.h"

namespace bound_write_writer_tests_h_
{

template <typename T>
struct Bar
{
    T value;

    operator T()
    {
        return value;
    }
};

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

    // SECTION("Float Tests")
    // {
    //     Foo<float> foo{4.4, 5.5, NULL};
    //     foo.eta.value = 6.6;
    //     printf("%s\n", bound::write::ToJson(foo).c_str());
    // }
}

} // namespace bound_write_writer_tests_h_

#endif