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


