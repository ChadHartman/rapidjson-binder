#ifndef BOUND_READ_READER_TESTS_HPP_
#define BOUND_READ_READER_TESTS_HPP_

#include "tests.h"

namespace bound_read_reader_tests_hpp_
{

template <typename T>
class Foo
{
private:
    T alpha_;

public:
    T beta;

    void set_alpha(T alpha)
    {
        alpha_ = alpha;
    }

    T alpha() const
    {
        return alpha_;
    }

    void gamma(T *gamma)
    {
        alpha_ = *gamma;
    }

    void delta(T &delta)
    {
        alpha_ = delta;
    }

    void epsilon(const T &epsilon)
    {
        alpha_ = epsilon;
    }

    constexpr static auto BOUND_PROPS_NAME = std::make_tuple(
        bound::property(&Foo::set_alpha, "alpha"),
        bound::property(&Foo::alpha, "alpha"),
        bound::property(&Foo::beta, "beta"),
        bound::property(&Foo::gamma, "gamma"),
        bound::property(&Foo::delta, "delta"),
        bound::property(&Foo::epsilon, "epsilon"));
};

struct Info
{
    std::string name;

    constexpr static auto BOUND_PROPS_NAME = std::make_tuple(
        bound::property(&Info::name, "name"));
};

struct Date
{
    long timestamp_ms;

    operator bound::JsonInt()
    {
        return bound::JsonInt{timestamp_ms};
    }

    Date &operator=(long timestamp)
    {
        timestamp_ms = timestamp;
        return *this;
    }
};

struct User
{
private:
    bool locked_ = true;

public:
    Info info;
    Date birthdate;
    std::vector<std::string> aliases;

    void set_locked(bool value)
    {
        locked_ = value;
    }

    bool locked()
    {
        return locked_;
    }

    constexpr static auto BOUND_PROPS_NAME = std::make_tuple(
        bound::property(&User::info, "info"),
        bound::property(&User::birthdate, "birthdate"),
        bound::property(&User::aliases, "aliases"),
        bound::property(&User::locked, "locked"),
        bound::property(&User::set_locked, "locked"));
};

struct DynObject
{

    std::string name;
    std::map<std::string, bound::JsonRaw> addl_props;

    constexpr static auto BOUND_PROPS_NAME = std::make_tuple(
        bound::property(&DynObject::name, "name"),
        bound::property(&DynObject::addl_props));
};

TEST_CASE("Reader Tests", "[reader_tests]")
{
    SECTION("User creation")
    {
        User user = bound::CreateWithJson<User>(
                        "{\"info\":{\"name\":\"alpha\"},"
                        "\"birthdate\":17,"
                        "\"aliases\":[\"beta\",\"gamma\"],"
                        "\"locked\":false}")
                        .instance;

        REQUIRE(user.info.name == "alpha");
        REQUIRE(user.birthdate.timestamp_ms == 17);
        REQUIRE(user.locked() == false);

        REQUIRE(std::is_same<Info, bound::read::ReadTarget<decltype(&User::info)>::type>::value);
        REQUIRE(std::is_same<bool, bound::read::ReadTarget<decltype(&User::set_locked)>::type>::value);
        REQUIRE(std::is_same<bound::read::Unassignable, bound::read::ReadTarget<decltype(&User::locked)>::type>::value);
    }

    SECTION("Obj with ints")
    {
        Foo<int> item = bound::CreateWithJson<Foo<int>>(
                            "{"
                            "\"alpha\":1,"
                            "\"beta\":2,"
                            "\"gamma\":3,"
                            "\"delta\":4,"
                            "\"epsilon\":5"
                            "}")
                            .instance;

        REQUIRE(5 == item.alpha());
        REQUIRE(2 == item.beta);
    }

    SECTION("Obj with map<string, int>")
    {
        Foo<std::map<std::string, bound::JsonInt>>
            item = bound::CreateWithJson<Foo<std::map<std::string, bound::JsonInt>>>(
                       "{"
                       "\"alpha\":{\"i\":1},"
                       "\"beta\":{\"i\":2},"
                       "\"gamma\":{\"i\":3},"
                       "\"delta\":{\"i\":4},"
                       "\"epsilon\":{\"i\":5}"
                       "}")
                       .instance;

        REQUIRE(5 == item.alpha().at("i").value);
        REQUIRE(2 == item.beta.at("i").value);
    }

    SECTION("Obj with map<string, json>")
    {
        Foo<std::map<std::string, bound::JsonRaw>>
            item = bound::CreateWithJson<Foo<std::map<std::string, bound::JsonRaw>>>(
                       "{"
                       "\"alpha\":{\"i\":{\"value\":1}},"
                       "\"beta\":{\"i\":{\"value\":2}},"
                       "\"gamma\":{\"i\":{\"value\":3}},"
                       "\"delta\":{\"i\":{\"value\":4}},"
                       "\"epsilon\":{\"i\":{\"value\":5}}"
                       "}")
                       .instance;

        REQUIRE("{\"value\":5}" == item.alpha().at("i").value);
        REQUIRE("{\"value\":2}" == item.beta.at("i").value);
    }

    SECTION("int")
    {
        REQUIRE(1 == bound::CreateWithJson<int>("1").instance);
    }

    SECTION("string")
    {
        REQUIRE("value" == bound::CreateWithJson<std::string>("\"value\"").instance);
    }

    SECTION("map<string, string>")
    {
        using Json = std::map<std::string, std::string>;
        Json item = bound::CreateWithJson<Json>("{\"foo\":\"value\"}").instance;
        REQUIRE("value" == item.at("foo"));
    }

    SECTION("list<string>")
    {
        using Container = std::list<std::string>;
        Container item = bound::CreateWithJson<Container>("[\"foo\",\"bar\"]").instance;
        REQUIRE("foo" == item.front());
        REQUIRE("bar" == item.back());
    }

    SECTION("list<vector<string>>")
    {
        using Container = std::list<std::vector<std::string>>;
        Container item = bound::CreateWithJson<Container>("[[\"foo\",\"bar\"]]").instance;
        REQUIRE("foo" == item.front().front());
        REQUIRE("bar" == item.front().back());
    }

    SECTION("deque<int>")
    {
        using Container = std::deque<int>;
        Container item = bound::CreateWithJson<Container>("[-1,100]").instance;
        REQUIRE(-1 == item[0]);
        REQUIRE(100 == item[1]);
    }

    SECTION("vector<map<string, string>>")
    {
        using Json = std::vector<std::map<std::string, std::string>>;
        Json item = bound::CreateWithJson<Json>("[{\"foo\":\"bar\"}]").instance;
        REQUIRE("bar" == item[0]["foo"]);
    }

    SECTION("DynObject")
    {
        DynObject o = bound::CreateWithJson<DynObject>(
                          "{"
                          "\"name\":\"John\","
                          "\"hometown\":\"San Diego\","
                          "\"age\":32,"
                          "\"aliases\":[\"Johnny\",\"Nathan\"],"
                          "\"demographics\":{"
                          "\"race\":\"purple\""
                          "}"
                          "}")
                          .instance;

        REQUIRE("John" == o.name);
        REQUIRE("\"San Diego\"" == o.addl_props.at("hometown").value);
        REQUIRE("32" == o.addl_props.at("age").value);
        REQUIRE("[\"Johnny\",\"Nathan\"]" == o.addl_props.at("aliases").value);
        REQUIRE("{\"race\":\"purple\"}" == o.addl_props.at("demographics").value);
    }

    SECTION("bound::JsonString")
    {

        auto test = [](std::string json, std::string expected) {
            bound::JsonString value;
            REQUIRE(bound::read::FromJson(json, value).success());
            REQUIRE(expected == value.value);
        };

        test("\"foo\"", "foo");
        test("true", "true");
        test("22.22", "22.220000");
        test("11", "11");
        test("-11", "-11");
    }

    SECTION("bound::JsonBool")
    {

        auto test = [](std::string json, bool expected) {
            bound::JsonBool value;
            REQUIRE(bound::read::FromJson(json, value).success());
            REQUIRE(expected == value.value);
        };

        test("true", true);
        test("22.22", true);
        test("0.0", false);
        test("0", false);
        test("234", true);
    }
}

} // namespace bound_read_reader_tests_hpp_

#endif
