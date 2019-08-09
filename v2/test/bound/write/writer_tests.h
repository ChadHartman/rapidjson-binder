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

    Foo(T alpha, T beta, T *gamma)
        : alpha_{alpha}, gamma_{gamma}, beta{beta} {}

    T alpha()
    {
        return alpha_;
    }

    T *gamma()
    {
        return gamma_;
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

    Bar<T> eta;

    constexpr static auto BOUND_PROPS_NAME = std::make_tuple(
        bound::property(&Foo::alpha, "alpha"),
        bound::property(&Foo::beta, "beta"),
        bound::property(&Foo::gamma, "gamma"),
        bound::property(&Foo::delta, "delta"),
        bound::property(&Foo::epsilon, "epsilon"),
        bound::property(&Foo::epsilon, "zeta"),
        bound::property(&Foo::eta, "eta"));
};

TEST_CASE("Writer Tests", "[writer_tests]")
{
    //std::string gamma{"foo"};
    //Foo<std::string> foo{"hello", "world", &gamma};
    Foo<std::string> foo{"hello", "world", NULL};
    foo.eta.value = "eta_value";
    printf("%s\n", bound::write::ToJson(foo).c_str());
}

} // namespace bound_write_writer_tests_h_

#endif