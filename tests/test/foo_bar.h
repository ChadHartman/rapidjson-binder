#ifndef FOO_BAR_H_
#define FOO_BAR_H_

#include "bound/bound.h"

struct Bar
{
    std::string s;
    std::vector<int> v;
    int i = 0;

    constexpr static auto properties = std::make_tuple(
        bound::property(&Bar::s, "string"),
        bound::property(&Bar::v, "array"),
        bound::property(&Bar::i, "integer"));
};

class Foo
{
private:
    Bar bar_;
    bound::JsonProperties json_properties_;

public:
    Bar &bar()
    {
        return bar_;
    }

    void set_bar(Bar &bar)
    {
        bar_ = bar;
    }

    constexpr static auto properties = std::make_tuple(
        bound::property(&Foo::bar_, "bar_property"),
        bound::property(&Foo::bar, "bar"),
        bound::property(&Foo::set_bar, "bar"),
        bound::property(&Foo::json_properties_));
};

#endif