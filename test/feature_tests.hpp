#ifndef TEST_FEATURE_TESTS_HPP_
#define TEST_FEATURE_TESTS_HPP_

#include "tests.h"

namespace test_feature_tests_hpp_supported_types
{

template <typename F, typename V = bound::JsonRaw>
struct Foo
{
    F bar;
    std::map<std::string, V> addl_props;

    constexpr static auto BOUND_PROPS_NAME = std::make_tuple(
        bound::property(&Foo::bar, "bar"),
        bound::property(&Foo::addl_props));
};

template <typename T>
void test_seq_container_int()
{
    const static std::string json = "[-1,0,1]";
    T v = {3, 2, 1};
    bound::UpdateWithJson(v, json);
    REQUIRE(json == bound::ToJson(v));
}

template <typename T>
void test_seq_container_str()
{
    const static std::string json = "[\"-1\",\"0\",\"1\"]";
    T v = {"3", "2", "1"};
    bound::UpdateWithJson(v, json);
    REQUIRE(json == bound::ToJson(v));
}

TEST_CASE("Supported Types", "[supported_types]")
{

    SECTION("int")
    {
        int a;
        bound::UpdateWithJson(a, "17");
        REQUIRE(17 == a);

        bound::UpdateWithJson(a, "null");
        REQUIRE(0 == a);
    }

    SECTION("unsigned")
    {
        unsigned a;
        bound::UpdateWithJson(a, "17");
        REQUIRE(17 == a);
    }

    SECTION("long")
    {
        long a;
        bound::UpdateWithJson(a, "17");
        REQUIRE(17 == a);
    }

    SECTION("float")
    {
        float a;
        bound::UpdateWithJson(a, "17.17");
        REQUIRE(17.17f == a);
    }

    SECTION("double")
    {
        double a;
        bound::UpdateWithJson(a, "17.17");
        REQUIRE(17.17 == a);
    }

    SECTION("bool")
    {
        bool a = false;
        bound::UpdateWithJson(a, "true");
        REQUIRE(a);
    }

    SECTION("std::vector")
    {
        test_seq_container_int<std::vector<int>>();
        test_seq_container_str<std::vector<std::string>>();
    }

    SECTION("std::list")
    {
        test_seq_container_int<std::list<int>>();
        test_seq_container_str<std::list<std::string>>();
    }

    SECTION("std::deque")
    {
        test_seq_container_int<std::deque<int>>();
        test_seq_container_str<std::deque<std::string>>();
    }

    SECTION("std::string")
    {
        std::string v = "foo";
        bound::UpdateWithJson(v, "\"bar\"");
        REQUIRE(v == "bar");
    }

    SECTION("std::map<std::string, std::string>")
    {
        std::map<std::string, std::string> v = {{"foo", "bar"}};
        bound::UpdateWithJson(v, "{\"alpha\":\"beta\",\"gamma\":\"delta\"}");
        REQUIRE(v.size() == 2);
        REQUIRE(v["alpha"] == "beta");
        REQUIRE(v["gamma"] == "delta");
    }

    SECTION("bound::JsonFloat")
    {
        bound::JsonFloat v;
        bound::UpdateWithJson(v, "22.22");
        REQUIRE(v.value == 22.22);
        REQUIRE("22.22" == bound::ToJson(v));

        bound::UpdateWithJson(v, "null");
        REQUIRE(0.0 == v.value);
    }

    SECTION("bound::JsonBool")
    {
        bound::JsonBool v;
        bound::UpdateWithJson(v, "false");
        REQUIRE(v.value == false);
        REQUIRE("false" == bound::ToJson(v));

        bound::UpdateWithJson(v, "null");
        REQUIRE(false == v.value);
    }

    SECTION("bound::JsonUint")
    {
        bound::JsonUint v;
        bound::UpdateWithJson(v, "12345");
        REQUIRE(v.value == 12345);
        REQUIRE("12345" == bound::ToJson(v));

        bound::UpdateWithJson(v, "null");
        REQUIRE(0 == v.value);
    }

    SECTION("bound::JsonInt")
    {
        bound::JsonInt v;
        bound::UpdateWithJson(v, "-12345");
        REQUIRE(v.value == -12345);
        REQUIRE("-12345" == bound::ToJson(v));

        bound::UpdateWithJson(v, "null");
        REQUIRE(0 == v.value);
    }

    SECTION("bound::JsonString")
    {
        const static std::string value = "\"Hello, world!\"";
        bound::JsonString v;
        bound::UpdateWithJson(v, value);
        REQUIRE(v.value == value.substr(1, value.length() - 2));
        REQUIRE(value == bound::ToJson(v));

        bound::UpdateWithJson(v, "null");
        REQUIRE("" == v.value);
    }

    SECTION("bound::JsonRaw")
    {
        bound::JsonRaw v;

        auto test = [](bound::JsonRaw &v, std::string &&json) {
            bound::UpdateWithJson(v, json);
            REQUIRE(json == v.value);
            REQUIRE(json == bound::ToJson(v));
        };

        test(v, "{\"message\":\"hello, world!\"}");
        test(v, "null");
        test(v, "22.33");
        test(v, "-11");
        test(v, "11");
        test(v, "[11]");
        test(v, "\"foo\"");
        test(v, "[\"foo\"]");
    }
}

TEST_CASE("Property Declaration", "[property_declaration]")
{
    SECTION("Known Properties")
    {
        const std::string json = "{\"bar\":17}";
        Foo<int> foo = bound::CreateWithJson<Foo<int>>(json).instance;
        REQUIRE(17 == foo.bar);
        REQUIRE(json == bound::ToJson(foo));
    }

    SECTION("Dynamic Properties[Known Value Types]")
    {
        const std::string json = "{\"bar\":17,\"unknown_key\":18}";
        Foo<int, int> foo = bound::CreateWithJson<Foo<int, int>>(json).instance;
        REQUIRE(17 == foo.bar);
        REQUIRE(18 == foo.addl_props["unknown_key"]);
        REQUIRE(json == bound::ToJson(foo));
    }

    SECTION("Dynamic Properties[Unnown Value Types]")
    {
        const std::string json = "{\"bar\":17,\"unknown_key_a\":18,\"unknown_key_b\":[19],\"unknown_key_c\":\"baz\"}";
        Foo<int> foo = bound::CreateWithJson<Foo<int>>(json).instance;
        REQUIRE(17 == foo.bar);
        REQUIRE("18" == foo.addl_props["unknown_key_a"].value);
        REQUIRE("[19]" == foo.addl_props["unknown_key_b"].value);
        REQUIRE("\"baz\"" == foo.addl_props["unknown_key_c"].value);
        REQUIRE(json == bound::ToJson(foo));
    }
}

namespace raw_json
{

struct Child
{
private:
    std::string raw_json;

public:
    operator bound::JsonRaw()
    {
        bound::JsonRaw raw_json_obj;
        raw_json_obj.value = raw_json;
        return raw_json_obj;
    }

    Child &operator=(const bound::JsonRaw &value)
    {
        raw_json = value.value;
        return *this;
    }
};

struct Parent
{
    // Operator called with "[\"Some\", \"Value\"]"
    Child child;

    constexpr static auto BOUND_PROPS_NAME = std::make_tuple(
        bound::property(&Parent::child, "json_name"));
};

TEST_CASE("Raw Json", "[raw_json]")
{

    SECTION("From Parent")
    {
        const std::string json = "{\"bar\":{\"some\":[\"dynamic\",{\"object\":\"value\"}]}}";
        Foo<bound::JsonRaw> foo = bound::CreateWithJson<Foo<bound::JsonRaw>>(json).instance;
        REQUIRE("{\"some\":[\"dynamic\",{\"object\":\"value\"}]}" == foo.bar.value);
        REQUIRE(json == bound::ToJson(foo));
    }

    SECTION("From Child")
    {
        const std::string json = "{\"json_name\":[\"Some\",\"Value\"]}";
        bound::ReadStatus status;
        Parent parent = bound::CreateWithJson<Parent>(json).instance;
        bound::JsonRaw child = (bound::JsonRaw)parent.child;
        REQUIRE("[\"Some\",\"Value\"]" == child.value);
        REQUIRE(json == bound::ToJson(parent));
    }
}

} // namespace raw_json

namespace getter
{

struct ParentOnly
{

    std::string get()
    {
        return "string_value";
    }

    constexpr static auto BOUND_PROPS_NAME = std::make_tuple(
        bound::property(&ParentOnly::get, "json_name"));
};

struct Child
{
    operator bound::JsonString()
    {
        return bound::JsonString{"value"};
    }
};

struct Parent
{
    Child child;

    constexpr static auto BOUND_PROPS_NAME = std::make_tuple(
        bound::property(&Parent::child, "json_name"));
};

TEST_CASE("Getter", "[getter]")
{
    SECTION("Parent Only")
    {
        ParentOnly parent;
        REQUIRE("{\"json_name\":\"string_value\"}" == bound::ToJson(parent));
    }

    SECTION("Parent and Child")
    {
        Parent parent;
        REQUIRE("{\"json_name\":\"value\"}" == bound::ToJson(parent));
    }
}

} // namespace getter

namespace setter
{

struct ParentOnly
{
    std::string value;

    void set(std::string &s)
    {
        value = s;
    }

    constexpr static auto BOUND_PROPS_NAME = std::make_tuple(
        bound::property(&ParentOnly::set, "json_name"));
};

struct Child
{

    std::string value;

    Child &operator=(const std::string &s)
    {
        value = s;
        return *this;
    }
};

struct Parent
{
    Child child;

    constexpr static auto BOUND_PROPS_NAME = std::make_tuple(
        bound::property(&Parent::child, "json_name"));
};

TEST_CASE("Setter", "[setter]")
{
    SECTION("Parent only")
    {
        std::string json = "{\"json_name\":\"foo\"}";
        ParentOnly parent = bound::CreateWithJson<ParentOnly>(json).instance;
        REQUIRE("foo" == parent.value);
    }

    SECTION("Parent and Child")
    {
        std::string json = "{\"json_name\":\"foo\"}";
        Parent parent = bound::CreateWithJson<Parent>(json).instance;
        REQUIRE("foo" == parent.child.value);
    }
}

} // namespace setter

TEST_CASE("Conditional Writing", "[conditional_writing]")
{
    SECTION("Whitespace")
    {
        bound::WriteConfig write_config;
        std::map<std::string, std::string> json = {{"message", "Hello, World!"}, {"foo", "bar"}};
        REQUIRE("{\n"
                "  \"foo\": \"bar\",\n"
                "  \"message\": \"Hello, World!\"\n"
                "}" == bound::ToJson(json, write_config.SetPrefix("  ")));
    }

    SECTION("Dec spaces")
    {
        bound::WriteConfig write_config;
        std::map<std::string, double> json = {{"alpha", 22.2222}, {"beta", -33.3333}};
        REQUIRE("{\"alpha\":22.22,\"beta\":-33.33}" == bound::ToJson(json, write_config.SetMaxDecimalPlaces(2)));
    }
}

} // namespace test_feature_tests_hpp_supported_types

// ## Conditional Writing

// Specific values can be filtered out using `bound::WriteConfig`. Alternatively each of the `bound::Json*` types possess a `bool render` field; which when set to `false`, will be skipped.

#endif