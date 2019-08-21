#ifndef BOUND_READ_READER_TESTS_HPP_
#define BOUND_READ_READER_TESTS_HPP_

#include "tests.h"

namespace bound_read_reader_tests_hpp_
{

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

TEST_CASE("Reader Tests", "[reader_tests]")
{
    User user = bound::read::FromJson<User>(
        "{\"info\":{\"name\":\"alpha\"},"
        "\"birthdate\":17,"
        "\"aliases\":[\"beta\",\"gamma\"],"
        "\"locked\":false}");

    printf("%s\n", bound::write::ToJson(user, bound::WriteConfig()).c_str());

    REQUIRE(user.info.name == "alpha");
    REQUIRE(user.birthdate.timestamp_ms == 17);
    REQUIRE(user.locked() == false);

    REQUIRE(std::is_same<Info, bound::read::ReadTarget<decltype(&User::info)>::type>::value);
    REQUIRE(std::is_same<bool, bound::read::ReadTarget<decltype(&User::set_locked)>::type>::value);
    REQUIRE(std::is_same<bound::read::Unassignable, bound::read::ReadTarget<decltype(&User::locked)>::type>::value);
}

} // namespace bound_read_reader_tests_hpp_

#endif
