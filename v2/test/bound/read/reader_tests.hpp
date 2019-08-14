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

struct User
{
private:
    bool locked_ = true;

public:
    Info info;

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
        bound::property(&User::locked, "locked"),
        bound::property(&User::set_locked, "locked"));
};

TEST_CASE("Reader Tests", "[reader_tests]")
{
    User user = bound::read::FromJson<User>("{\"info\":{\"name\":\"alpha\"},\"locked\":false}");
    REQUIRE(user.info.name == "alpha");
    REQUIRE(user.locked() == false);

    REQUIRE(std::is_same<Info, bound::read::ReadTarget<decltype(&User::info)>::type>::value);
    REQUIRE(std::is_same<bool, bound::read::ReadTarget<decltype(&User::set_locked)>::type>::value);
    REQUIRE(std::is_same<bound::read::Unassignable, bound::read::ReadTarget<decltype(&User::locked)>::type>::value);
}

} // namespace bound_read_reader_tests_hpp_

#endif
