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

    bound::CreateStatus<Foo> status = bound::CreateWithJson<Foo>(json);
    Foo foo = status.instance;

    if (status.success)
    {
        printf("Parsed: %s\n", bound::ToJson(foo).c_str());
        // Enhanced writing options are found in bound::WriteConfig:
        // printf("Parsed: %s\n", bound::ToJson(foo, bound::WriteConfig().SetPrefix("  ")).c_str());
    }
    else
    {
        printf("Failed to create: \"%s\"\n", status.error_message.c_str());
    }

    assert(foo.bar.value == "bar_value");
    assert(foo.numbers.size() == 3);
    assert(foo.numbers[0] == 1);
    assert(foo.numbers[1] == 2);
    assert(foo.numbers[2] == 3);
    assert(json == bound::ToJson(foo));

    printf("All assertions correct!\n");

    return 0;
}