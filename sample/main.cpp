#include "bound/bound.h"

#include <vector>
#include <deque>
#include <list>

using namespace bound;

struct Bar
{
private:
    std::deque<std::string> list_string_private_;
    std::deque<std::string> list_string_property_;

    std::string string_private_;
    std::string string_property_;

public:
    std::deque<std::string> &list_string_getter() { return list_string_private_; };
    void list_string_setter(std::deque<std::string> &value) { list_string_private_ = value; };

    std::string &string_getter() { return string_private_; };
    void string_setter(std::string value) { string_private_ = value; };

    constexpr static auto properties = std::make_tuple(
        property(&Bar::list_string_property_, "list_string_property"),
        property(&Bar::list_string_getter, "list_string_getter_setter"),
        property(&Bar::list_string_setter, "list_string_getter_setter"),

        property(&Bar::string_property_, "string_property"),
        property(&Bar::string_getter, "string_getter_setter"),
        property(&Bar::string_setter, "string_getter_setter"));
};

struct Foo
{
private:
    Bar bar_private_;
    Bar bar_property_;

    bool bool_private_;
    bool bool_property_;

    float float_private_;
    float float_property_;

    int int_private_;
    int int_property_;

    std::list<Bar> list_bar_private_;
    std::list<Bar> list_bar_property_;

    std::vector<bool> list_bool_private_;
    std::vector<bool> list_bool_property_;

    unsigned long ulong_private_;
    unsigned long ulong_property_;

public:
    Bar &bar_getter() { return bar_private_; };
    void bar_setter(Bar value) { bar_private_ = value; };

    bool bool_getter() { return bool_private_; };
    void bool_setter(bool value) { bool_private_ = value; };

    float float_getter() { return float_private_; };
    void float_setter(float value) { float_private_ = value; };

    int int_getter() { return int_private_; };
    void int_setter(int value) { int_private_ = value; };

    std::list<Bar> &list_bar_getter() { return list_bar_private_; };
    void list_bar_setter(std::list<Bar> &value) { list_bar_private_ = value; };

    std::vector<bool> &list_bool_getter() { return list_bool_private_; };
    void list_bool_setter(std::vector<bool> &value) { list_bool_private_ = value; };

    unsigned long ulong_getter() { return ulong_private_; };
    void ulong_setter(unsigned long value) { ulong_private_ = value; };

    constexpr static auto properties = std::make_tuple(

        property(&Foo::bar_property_, "bar_property"),
        property(&Foo::bar_getter, "bar_getter_setter"),
        property(&Foo::bar_setter, "bar_getter_setter"),

        property(&Foo::bool_property_, "bool_property"),
        property(&Foo::bool_getter, "bool_getter_setter"),
        property(&Foo::bool_setter, "bool_getter_setter"),

        property(&Foo::float_property_, "float_property"),
        property(&Foo::float_getter, "float_getter_setter"),
        property(&Foo::float_setter, "float_getter_setter"),

        property(&Foo::int_property_, "int_property"),
        property(&Foo::int_getter, "int_getter_setter"),
        property(&Foo::int_setter, "int_getter_setter"),

        property(&Foo::list_bar_property_, "list_bar_property"),
        property(&Foo::list_bar_getter, "list_bar_getter_setter"),
        property(&Foo::list_bar_setter, "list_bar_getter_setter"),

        property(&Foo::list_bool_property_, "list_bool_property"),
        property(&Foo::list_bool_getter, "list_bool_getter_setter"),
        property(&Foo::list_bool_setter, "list_bool_getter_setter"),

        property(&Foo::ulong_property_, "ulong_property"),
        property(&Foo::ulong_getter, "ulong_getter_setter"),
        property(&Foo::ulong_setter, "ulong_getter_setter"));
};

int main()
{
    ReadStatus status;
    Foo test;

    printf("Reading \"test.json\" to object...\n");
    status = FromJsonFile("test.json", test);
    printf("\tSuccess: \"%s\"; error: \"%s\"\n", status.success() ? "true" : "false", status.error_message().c_str());
    if (!status.success())
    {
        return -1;
    }

    std::string test_json = ToJson(test);
    printf("\tUser: %s\n\n", test_json.c_str());

    printf("Now Reading from string to object...\n");
    status = FromJsonFile(test_json, test);
    printf("\tSuccess: \"%s\"; error: \"%s\"\n\n", status.success() ? "true" : "false", status.error_message().c_str());
    if (!status.success())
    {
        return -1;
    }

    printf("Now writing to \"temp.json\"...\n\n");
    ToJsonFile(test, "temp.json");

    printf("Now attempting malformed read from string...\n");
    std::string malformed_json = "{\"string_property\": 6";
    status = FromJson(malformed_json, test);
    printf("\tSuccess: \"%s\"; error: \"%s\"\n", status.success() ? "true" : "false", status.error_message().c_str());

    return 0;
}