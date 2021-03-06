#ifndef TEST_INCLUDE_BOUND_TESTS_H_
#define TEST_INCLUDE_BOUND_TESTS_H_

#include <fstream>
#include <sstream>

namespace test_include_bound_tests_h_
{

const std::string ReadFile(const std::string filename)
{
    std::ifstream t(filename);
    std::stringstream buffer;
    buffer << t.rdbuf();
    return buffer.str();
}

struct Foo
{
    int value;

    constexpr static auto BOUND_PROPS_NAME = std::make_tuple(
        bound::property(&Foo::value, "value"));
};

TEST_CASE("CreateWithJson", "[create_with_json]")
{
    SECTION("CreateStatus<T> CreateWithJson(const std::string &json)")
    {
        const std::string json = "{\"value\":17}";
        auto status = bound::CreateWithJson<Foo>(json);
        REQUIRE(status.success);
        REQUIRE(0 == status.error_message.length());
        REQUIRE(17 == status.instance.value);
        // Demonstate it's mutable
        status.instance.value = 18;
    }

    SECTION("CreateStatus<T> CreateWithJson(const std::string &&json)")
    {
        auto status = bound::CreateWithJson<Foo>("{\"value\":17}");
        REQUIRE(status.success);
        REQUIRE(0 == status.error_message.length());
        REQUIRE(17 == status.instance.value);
        // Demonstate it's mutable
        status.instance.value = 18;
    }

    SECTION("CreateWithJson invalid json")
    {
        auto status = bound::CreateWithJson<Foo>("{\"value\":}");
        REQUIRE(!status.success);
        REQUIRE(0 < status.error_message.length());
    }
}

TEST_CASE("CreateWithJsonFile", "[create_with_json_file]")
{
    const std::string path = "test_temp.json";

    SECTION("CreateStatus<T> CreateWithJsonFile(const std::string &path)")
    {
        Foo foo_original;
        foo_original.value = 22;
        bound::ToJsonFile(foo_original, path);

        auto status = bound::CreateWithJsonFile<Foo>(path);
        REQUIRE(status.success);
        REQUIRE(0 == status.error_message.length());
        REQUIRE(22 == status.instance.value);
        // Demonstate it's mutable
        status.instance.value = 18;
    }

    SECTION("CreateStatus<T> CreateWithJsonFile(const std::string &&path)")
    {
        Foo foo_original;
        foo_original.value = 23;
        bound::ToJsonFile(foo_original, path);

        auto status = bound::CreateWithJsonFile<Foo>("test_temp.json");
        REQUIRE(status.success);
        REQUIRE(0 == status.error_message.length());
        REQUIRE(23 == status.instance.value);
        // Demonstate it's mutable
        status.instance.value = 18;
    }

    SECTION("CreateWithJsonFile invalid json")
    {
        auto status = bound::CreateWithJsonFile<Foo>("malformed.json");
        REQUIRE(!status.success);
        REQUIRE(0 < status.error_message.length());
    }
}

TEST_CASE("UpdateWithJson", "[update_with_json]")
{
    Foo foo = {3};

    SECTION("UpdateStatus UpdateWithJson(T &instance, const std::string &json)")
    {
        const std::string json = "{\"value\":17}";
        auto status = bound::UpdateWithJson(foo, json);
        REQUIRE(status.success);
        REQUIRE(0 == status.error_message.length());
        REQUIRE(17 == foo.value);
    }

    SECTION("UpdateStatus UpdateWithJson(T &instance, const std::string &&json)")
    {
        auto status = bound::UpdateWithJson(foo, "{\"value\":18}");
        REQUIRE(status.success);
        REQUIRE(0 == status.error_message.length());
        REQUIRE(18 == foo.value);
    }

    SECTION("UpdateWithJson invalid json")
    {
        auto status = bound::UpdateWithJson(foo, "{\"value\":}");
        REQUIRE(!status.success);
        REQUIRE(0 < status.error_message.length());
    }
}

TEST_CASE("UpdateWithJsonFile", "[update_with_json_file]")
{
    Foo foo_mutable = {0};
    const std::string path = "test_temp.json";

    SECTION("UpdateStatus UpdateWithJsonFile(T &instance, const std::string &json)")
    {
        const Foo foo_original = {32};
        bound::ToJsonFile(foo_original, path);

        auto status = bound::UpdateWithJsonFile(foo_mutable, path);
        REQUIRE(status.success);
        REQUIRE(0 == status.error_message.length());
        REQUIRE(32 == foo_mutable.value);
    }

    SECTION("UpdateStatus UpdateWithJsonFile(T &instance, const std::string &&json)")
    {
        const Foo foo_original = {33};
        bound::ToJsonFile(foo_original, path);

        auto status = bound::UpdateWithJsonFile(foo_mutable, "test_temp.json");
        REQUIRE(status.success);
        REQUIRE(0 == status.error_message.length());
        REQUIRE(33 == foo_mutable.value);
    }

    SECTION("UpdateWithJsonFile invalid json")
    {
        auto status = bound::UpdateWithJsonFile(foo_mutable, "malformed.json");
        REQUIRE(!status.success);
        REQUIRE(0 < status.error_message.length());
    }
}

TEST_CASE("ToJson", "[to_json]")
{
    Foo foo = {3};

    SECTION("const std::string ToJson(T &instance, const WriteConfig &config)")
    {
        bound::WriteConfig config;
        REQUIRE("{\"value\":3}" == bound::ToJson(foo, config));
    }

    SECTION("const std::string ToJson(T &instance, const WriteConfig &&config)")
    {
        REQUIRE("{\"value\":3}" == bound::ToJson(foo, bound::WriteConfig()));
    }

    SECTION("const std::string ToJson(T &instance)")
    {
        REQUIRE("{\"value\":3}");
    }
}

TEST_CASE("ToJsonFile", "[to_json_file]")
{
    const static std::string filename = "test_temp.json";

    SECTION("const bool ToJsonFile(T&,const WriteConfig&)")
    {
        Foo foo = {1};
        bound::WriteConfig config = bound::WriteConfig().SetFilename(filename);
        REQUIRE(bound::ToJsonFile(foo, config));
        REQUIRE("{\"value\":1}" == ReadFile(filename));
    }

    SECTION("const bool ToJsonFile(T&,const WriteConfig&&)")
    {
        Foo foo = {2};
        REQUIRE(bound::ToJsonFile(foo, bound::WriteConfig().SetFilename(filename)));
        REQUIRE("{\"value\":2}" == ReadFile(filename));
    }

    SECTION("const bool ToJsonFile(T&,const std::string&)")
    {
        Foo foo = {3};
        REQUIRE(bound::ToJsonFile(foo, filename));
        REQUIRE("{\"value\":3}" == ReadFile(filename));
    }

    SECTION("const bool ToJsonFile(T&,const std::string&&)")
    {
        Foo foo = {4};
        REQUIRE(bound::ToJsonFile(foo, "test_temp.json"));
        REQUIRE("{\"value\":4}" == ReadFile(filename));
    }

    SECTION("ToJsonFile no filename given")
    {
        Foo foo = {4};
        REQUIRE(!bound::ToJsonFile(foo, bound::WriteConfig()));
    }
}

} // namespace test_include_bound_tests_h_

#endif