#include "bound/bound.h"

struct Bar
{
    std::string value;

    constexpr static auto BOUND_PROPS_NAME = std::make_tuple(
        bound::property(&Bar::value, "value"));
};

struct Foo
{
    Bar bar;
    std::vector<int> numbers;

    constexpr static auto BOUND_PROPS_NAME = std::make_tuple(
        bound::property(&Foo::bar, "bar"),
        bound::property(&Foo::numbers, "numbers"));
};

int main()
{
    const std::string json =
        "{"
        "\"bar\":{\"value\":\"bar_value\"},"
        "\"numbers\":[1,2,3]"
        "}";

    Foo foo = bound::FromJson<Foo>(json);
    assert(foo.bar.value == "bar_value");
    assert(foo.numbers.size() == 3);
    assert(foo.numbers[0] == 1);
    assert(foo.numbers[1] == 2);
    assert(foo.numbers[2] == 3);
    assert(json == bound::ToJson(foo));

    printf("All assertions correct!\n");

    return 0;
}