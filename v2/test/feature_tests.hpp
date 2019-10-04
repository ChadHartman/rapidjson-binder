#ifndef TEST_FEATURE_TESTS_HPP_
#define TEST_FEATURE_TESTS_HPP_

#include "tests.h"

namespace test_feature_tests_hpp_supported_types
{

TEST_CASE("Supported Types", "[supported_types]")
{

    SECTION("int")
    {
        int a;
        bound::FromJson("17", a);
        REQUIRE(17 == a);
    }

    SECTION("unsigned")
    {
        unsigned a;
        bound::FromJson("17", a);
        REQUIRE(17 == a);
    }

    SECTION("long")
    {
        long a;
        bound::FromJson("17", a);
        REQUIRE(17 == a);
    }

    SECTION("float")
    {
        float a;
        bound::FromJson("17.17", a);
        REQUIRE(17.17f == a);
    }

    SECTION("double")
    {
        double a;
        bound::FromJson("17.17", a);
        REQUIRE(17.17 == a);
    }

    SECTION("bool")
    {
        bool a = false;
        bound::FromJson("true", a);
        REQUIRE(a);
    }

    // * Sequential Containers: `std::vector`, `std::list`, `std::deque`
    // * `std::string`
    // * `std::map<K, V>`
    // * `class`/`struct` with `constexpr static std::tuple<...> properties` field
    // * `bound::JsonFloat`
    // * `bound::JsonBool`
    // * `bound::JsonUint`
    // * `bound::JsonInt`
    // * `bound::JsonString`
    // * `bound::JsonRaw`
}

} // namespace test_feature_tests_hpp_supported_types

// ## Property Declaration

// ### Known Properties

// JSON properties can be registered in a `constexpr static std::tuple<...> properties` field:

// ```
// // Example: {"bar":17}
// struct Foo {

//     // Value: 17
//     int bar;

//     constexpr static auto properties = std::make_tuple(
//         bound::property(&Foo::bar, "bar")
//     );
// };
// ```

// ### Dynamic Properties

// #### Known Value Types

// If a JSON object has a dynamic set of keys with known value type `T`, use a `std::map<std::string, T>` property:

// ```
// // Example: {"bar":17,"unknown_key":18}
// struct Foo {

//     // Value: 17
//     int bar;

//     // Value:
//     //  "unknown_key", 18
//     std::map<std::string, int> addl_props;

//     constexpr static auto properties = std::make_tuple(
//         bound::property(&Foo::bar, "bar"),
//         bound::property(&Foo::addl_props)
//     );
// };
// ```

// #### Unnown Value Types

// If a JSON object has a dynamic set of keys with an unknown value type, use a `std::map<std::string, bound::JsonRaw>` property:

// ```
// // Example: {"bar":17,"unknown_key_a":18,"unknown_key_b":[19],"unknown_key_c":"baz"}
// struct Foo {

//     // Value: 17
//     int bar;

//     // Value:
//     //  "unknown_key_a", "18"
//     //  "unknown_key_b", "[19]"
//     //  "unknown_key_c", "\"baz\""
//     std::map<std::string, bound::JsonRaw> addl_props;

//     constexpr static auto properties = std::make_tuple(
//         bound::property(&Foo::bar, "bar"),
//         bound::property(&Foo::addl_props)
//     );
// };
// ```

// `bound::JsonRaw`'s `value` field is a JSON-formatted `std::string` of the parsed value.

// ## RawJson

// Class: `bound::JsonRaw`. `bound::JsonRaw.value` is a `std::string` with a json-formatted string for arbitrary structures.

// ### From Parent

// ```
// // Example: {"json_name":{"some":["dynamic",{"object":"value"}]}}
// struct Parent {

//     // Value: "{\"some\":[\"dynamic\",{\"object\":\"value\"}]}"
//     bound::JsonRaw raw_json;

//     constexpr static auto properties = std::make_tuple(
//         bound::property(&Parent::raw_json, "json_name")
//     );
// };
// ```

// ### From Child

// ```
// struct Child {

//     operator bound::RawJson() {
//         return bound::RawJson("{}");
//     }

//     Child &operator=(bound::RawJson &value) {
//         return *this;
//     }

// };

// // Example: {"json_name":["Some", "Value"]}
// struct Parent {

//     // Operator called with "[\"Some\", \"Value\"]"
//     Child child;

//     constexpr static auto properties = std::make_tuple(
//         bound::property(&Parent::child, "json_name")
//     );
// };
// ```

// ## Getters

// ### Parent Getter

// ```
// // Readonly
// //  Returns {"json_name":"string value"}
// struct Parent {

//     std::string get() {
//         return "string value";
//     }

//     constexpr static auto properties = std::make_tuple(
//         bound::property(&Parent::get, "json_name")
//     );
// };
// ```

// ### Child Getter

// ```
// struct Child {

//     operator bound::JsonString() {
//         return bound::JsonString{"value"};
//     }

// };

// // Readonly
// //  Returns {"json_name":"value"}
// struct Parent {

//     Child child;

//     constexpr static auto properties = std::make_tuple(
//         bound::property(&Parent::child, "json_name")
//     );
// };
// ```

// ## Setters

// ### Parent Setter

// ```
// // On read returns {}
// // Can be set with: {"json_name":"foo"}
// struct Parent {

//     // value = "foo"
//     void set(std::string &value) {
//         //..
//     }

//     constexpr static auto properties = std::make_tuple(
//         bound::property(&Parent::set, "json_name")
//     );

// };
// ```

// ### Child Setter

// ```
// struct Child {

//     Child &operator =(const std::string& value) {
//         //...
//         return *this;
//     }

// };

// // On read returns {}
// // Can be set with: {"json_name":"foo"}
// struct Parent {

//     // Child's assignment operator will called with "foo"
//     Child child;

//     constexpr static auto properties = std::make_tuple(
//         bound::property(&Parent::child, "json_name")
//     );

// };
// ```

// ## Conditional Writing

// Specific values can be filtered out using `bound::WriteConfig`. Alternatively each of the `bound::Json*` types possess a `bool render` field; which when set to `false`, will be skipped.

#endif