# rapidjson-binder

Binds JSON Objects to C++ Objects using rapidjson.

## Versions

* [rapidjson](http://rapidjson.org): `1.1.0`.
* C++: `C++14`.

## Example

Located in `sample/main.cpp`:

```c++
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
```

## Design Philosophy

This library utilizes rapidjson's [SAX Deserializer](http://rapidjson.org/md_doc_sax.html) to tokenize JSON events and map object properties directly with no intermediate complex objects. 

This library follows a "fail fast" philosophy. Deserialization will immediately stop when an unexpected event occurs such as malformed json or incompatable value. JSON comments are ignored by the library. The passed in C++ object will be in an incomplete state.

Errors are reported in a `bound::CreateStatus` or `bound::UpdateStatus` value (depending on which operation is performed) which will provide a detailed messsage of where the error occurred.

## Features

Features list located in doc/features.md.

## Changelog

### [2.0.0] - 2019-12-03
#### Added
- Bound types for conditional rendering
- Support for writing `const` objects
- Support for pointer getters and setters
#### Removed
- "Authorizer" feature
    - Use `bound::JsonFloat`, `bound::JsonBool`, `bound::JsonUint`, `bound::JsonInt`, `bound::JsonString`, and `bound::JsonRaw` instead

### [1.3.0] - 2019-08-03
#### Added
- Custom `properties` name override
    - `#define BOUND_PROPS_NAME kAlternativePropertiesName` before including `bound.h`
- "Authorizer" feature
    - Register a `bool authorize(const char *)` class method in the properties object that returns whether to write a property with the name passed in the authorizer method

### [1.2.1] - 2019-07-09
#### Fixed
- `scanner.h:151:5 expects an l-value for 1st argument` bug

### [1.2.0] - 2019-07-01
#### Added
- Added support for unbound keys: 
    - Store in a `bound::DynamicProperties` collection
    - Recursively Skip if a `bound::DynamicProperties` is not provided

### [1.1.0] - 2019-06-28
#### Added
- Conditional writing support

### [1.0.0] - 2019-06-14
#### Added
- Initial Release
- Read/Write C++ objects from/to JSON
- Support for:
    - Primatives (`bool`, `int`, `double`, etc...)
    - `std::string`
    - `std::deque`
    - `std::list`
    - `std::vector`
    - Any `struct` or `class` with a public `constexpr static properties` `tuple` field.
    - `void set_property(T value)` and `void set_property(T &value)` type setters.
    - `void T property()` and `T& property()` type getters.
