# rapidjson-binder

Binds JSON Objects to C++ Objects using rapidjson.

## Versions

* [rapidjson](http://rapidjson.org): `1.1.0`.
* C++: `C++14`.

## Example

```c++

#include "bound/bound.h"

struct Sample
{
    int property;
    
    constexpr static auto properties = std::make_tuple(
            bound::property(&Sample::property, "property"));
};

int main() {
    
    Sample sample;
    
    bound::ReadStatus status = bound::FromJsonFile("sample.json", sample);
    if (!status.success())
    {
        printf("Error: %s\n", status.error_message().c_str());
        return -1;
    }
    
    printf("Sample was parsed as \"%s\".\n", bound::ToJson(sample).c_str());
    
    return 0;
}
```

## Design Philosophy

This library utilizes rapidjson's [SAX Deserializer](http://rapidjson.org/md_doc_sax.html) to tokenize JSON events and map object properties directly with no intermediate complex objects. 

This library follows a "fail fast" philosophy. Deserialization will immediately stop when an unexpected event occurs such as malformed json, incompatable value, or unknown key. JSON Comments are ignored by the library. The passed in C++ object will be in an incomplete state.

Errors are reported in a `bound::ReadStatus` return value which will provide a detailed messsage of where the error occurred.

## Supported C++ Types

* Primatives (`bool`, `int`, `double`, etc...)
* `std::string`
* `std::deque`
* `std::list`
* `std::vector`
* Any `struct` or `class` with a public `constexpr static properties` `tuple` field.
* `void set_property(T value)` and `void set_property(T &value)` type setters.
* `void T property()` and `T& property()` type getters.

## FAQ

### <Type> cannot be used prior to '::' because it has no members

If you see the error:

```
../include/bound/reader.h:135:58: error: type <Type> cannot be used prior to '::' because it has no members
    constexpr auto prop_count = std::tuple_size<decltype(T::properties)>::value;
```

Then the type cannot be de/serialized. For `class` and `struct`s, add a public `constexpr static auto properties = std::make_tuple();` field.

## Changelog

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
