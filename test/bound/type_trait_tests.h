#ifndef BOUND_TYPE_TRAIT_TESTS_H_
#define BOUND_TYPE_TRAIT_TESTS_H_

#include "tests.h"

namespace bound_type_trait_tests_h_
{

struct Bound
{
    std::map<std::string, bound::JsonRaw> addl_props;
    constexpr static auto BOUND_PROPS_NAME = std::make_tuple();
};

struct Unbound
{
    int alpha;
    Bound beta;
    int gamma() const { return 0; }
    Bound delta() { return Bound(); }
    void epsilon(int value) {}
    void zeta(Bound value) {}
    std::string eta(std::string input) const;
    void theta();
};

TEST_CASE("Type Trait Tests", "[type_trait_tests]")
{
    SECTION("is_bound")
    {
        REQUIRE(bound::is_bound<Bound>::value);
        REQUIRE(!bound::is_bound<Unbound>::value);
        REQUIRE(!bound::is_bound<int>::value);

        REQUIRE(!bound::is_bound<std::vector<int>>::value);
        REQUIRE(!bound::is_bound<std::vector<Bound>>::value);
        REQUIRE(!bound::is_bound<std::vector<Unbound>>::value);

        REQUIRE(!bound::is_bound<std::map<std::string, int>>::value);
        REQUIRE(!bound::is_bound<std::map<std::string, Bound>>::value);
        REQUIRE(!bound::is_bound<std::map<std::string, Unbound>>::value);
    }

    SECTION("is_seq_container")
    {
        REQUIRE(!bound::is_seq_container<Bound>::value);
        REQUIRE(!bound::is_seq_container<Unbound>::value);
        REQUIRE(!bound::is_seq_container<int>::value);

        REQUIRE(bound::is_seq_container<std::list<int>>::value);
        REQUIRE(bound::is_seq_container<std::vector<Bound>>::value);
        REQUIRE(bound::is_seq_container<std::deque<Unbound>>::value);

        REQUIRE(!bound::is_seq_container<std::map<std::string, int>>::value);
        REQUIRE(!bound::is_seq_container<std::map<std::string, Bound>>::value);
        REQUIRE(!bound::is_seq_container<std::map<std::string, Unbound>>::value);
    }

    SECTION("is_json_properties")
    {
        REQUIRE(!bound::is_json_properties<Bound>::value);
        REQUIRE(!bound::is_json_properties<Unbound>::value);
        REQUIRE(!bound::is_json_properties<int>::value);

        REQUIRE(!bound::is_json_properties<std::vector<int>>::value);
        REQUIRE(!bound::is_json_properties<std::vector<Bound>>::value);
        REQUIRE(!bound::is_json_properties<std::vector<Unbound>>::value);

        REQUIRE(bound::is_json_properties<std::map<std::string, int>>::value);
        REQUIRE(bound::is_json_properties<std::map<std::string, Bound>>::value);
        REQUIRE(bound::is_json_properties<std::map<std::string, Unbound>>::value);
    }

    SECTION("is_int")
    {
        REQUIRE(!bound::is_int<Bound>::value);
        REQUIRE(!bound::is_int<Unbound>::value);
        REQUIRE(!bound::is_int<std::vector<int>>::value);
        REQUIRE(!bound::is_int<std::map<std::string, int>>::value);
        REQUIRE(bound::is_int<int>::value);
        REQUIRE(bound::is_int<char>::value);
        REQUIRE(bound::is_int<long>::value);
        REQUIRE(bound::is_int<long long>::value);
        REQUIRE(bound::is_int<short>::value);
        REQUIRE(!bound::is_int<bool>::value);
        REQUIRE(!bound::is_int<double>::value);
        REQUIRE(!bound::is_int<float>::value);
        REQUIRE(!bound::is_int<unsigned>::value);
    }

    SECTION("is_uint")
    {
        REQUIRE(!bound::is_uint<Bound>::value);
        REQUIRE(!bound::is_uint<Unbound>::value);
        REQUIRE(!bound::is_uint<std::vector<unsigned>>::value);
        REQUIRE(!bound::is_uint<std::map<std::string, unsigned>>::value);
        REQUIRE(!bound::is_uint<int>::value);
        REQUIRE(!bound::is_uint<char>::value);
        REQUIRE(!bound::is_uint<long>::value);
        REQUIRE(!bound::is_uint<long long>::value);
        REQUIRE(!bound::is_uint<short>::value);
        REQUIRE(!bound::is_uint<bool>::value);
        REQUIRE(!bound::is_uint<double>::value);
        REQUIRE(!bound::is_uint<float>::value);
        REQUIRE(bound::is_uint<unsigned>::value);
        REQUIRE(bound::is_uint<unsigned long long>::value);
    }

    SECTION("is_getter")
    {
        REQUIRE(!bound::is_getter<decltype(&Unbound::alpha)>::value);
        REQUIRE(!bound::is_getter<decltype(&Unbound::beta)>::value);
        REQUIRE(bound::is_getter<decltype(&Unbound::gamma)>::value);
        REQUIRE(bound::is_getter<decltype(&Unbound::delta)>::value);
        REQUIRE(!bound::is_getter<decltype(&Unbound::epsilon)>::value);
        REQUIRE(!bound::is_getter<decltype(&Unbound::zeta)>::value);
        REQUIRE(!bound::is_getter<decltype(&Unbound::eta)>::value);
        REQUIRE(!bound::is_getter<decltype(&Unbound::theta)>::value);
    }

    SECTION("is_setter")
    {
        REQUIRE(!bound::is_setter<decltype(&Unbound::alpha)>::value);
        REQUIRE(!bound::is_setter<decltype(&Unbound::beta)>::value);
        REQUIRE(!bound::is_setter<decltype(&Unbound::gamma)>::value);
        REQUIRE(!bound::is_setter<decltype(&Unbound::delta)>::value);
        REQUIRE(bound::is_setter<decltype(&Unbound::epsilon)>::value);
        REQUIRE(bound::is_setter<decltype(&Unbound::zeta)>::value);
        REQUIRE(!bound::is_setter<decltype(&Unbound::eta)>::value);
        REQUIRE(!bound::is_setter<decltype(&Unbound::theta)>::value);
    }
}

} // namespace bound_type_trait_tests_h_

#endif