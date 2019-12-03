# Features

## Supported Types

* Arithmetic: `int`, `unsigned`, `long`, `float`, `double`, `bool`, etc
* Sequential Containers: `std::vector`, `std::list`, `std::deque` 
    * When parsed, the container is cleared first
* `std::string`
* `std::map<K, V>`
    * When parsed, the container is cleared first
* `class`/`struct` with `constexpr static std::tuple<...> properties` field and a default constructor
* `bound::Json*` objects:
    * `bound::JsonFloat`
    * `bound::JsonBool`
    * `bound::JsonUint`
    * `bound::JsonInt`
    * `bound::JsonString`
    * `bound::JsonRaw`
    *  Note: only available as fields and getters, not setters; except `bound::JsonRaw`. When a setter is necessary, use the base type (`bool`, `int`, `std::string`, etc).

## Property Declaration

### Known Properties

JSON properties can be registered in a `constexpr static std::tuple<...> properties` field:

```
// Example: {"bar":17}
struct Foo {

    // Value: 17
    int bar;

    constexpr static auto properties = std::make_tuple(
        bound::property(&Foo::bar, "bar")
    );
};
```

### Dynamic Properties

#### Known Value Types

If a JSON object has a dynamic set of keys with known value type `T`, use a `std::map<std::string, T>` property:

```
// Example: {"bar":17,"unknown_key":18}
struct Foo {

    // Value: 17
    int bar;

    // Value:
    //  "unknown_key", 18
    std::map<std::string, int> addl_props;

    constexpr static auto properties = std::make_tuple(
        bound::property(&Foo::bar, "bar"),
        bound::property(&Foo::addl_props)
    );
};
```

#### Unnown Value Types

If a JSON object has a dynamic set of keys with an unknown value type, use a `std::map<std::string, bound::JsonRaw>` property:

```
// Example: {"bar":17,"unknown_key_a":18,"unknown_key_b":[19],"unknown_key_c":"baz"}
struct Foo {

    // Value: 17
    int bar;

    // Value:
    //  "unknown_key_a", "18"
    //  "unknown_key_b", "[19]"
    //  "unknown_key_c", "\"baz\""
    std::map<std::string, bound::JsonRaw> addl_props;

    constexpr static auto properties = std::make_tuple(
        bound::property(&Foo::bar, "bar"),
        bound::property(&Foo::addl_props)
    );
};
```

`bound::JsonRaw`'s `value` field is a JSON-formatted `std::string` of the parsed value.


## Raw Json

Class: `bound::JsonRaw`. `bound::JsonRaw.value` is a `std::string` with a json-formatted string for arbitrary structures. 

### From Parent

```
// Example: {"json_name":{"some":["dynamic",{"object":"value"}]}}
struct Parent {

    // Value: "{\"some\":[\"dynamic\",{\"object\":\"value\"}]}"
    bound::JsonRaw raw_json;

    constexpr static auto properties = std::make_tuple(
        bound::property(&Parent::raw_json, "json_name")
    );
};
```

### From Child

```
struct Child {

    operator bound::JsonRaw() {
        return bound::JsonRaw("{}");
    }

    Child &operator=(bound::JsonRaw &value) {
        return *this;
    }

};

// Example: {"json_name":["Some", "Value"]}
struct Parent {

    // Operator called with "[\"Some\", \"Value\"]"
    Child child;

    constexpr static auto properties = std::make_tuple(
        bound::property(&Parent::child, "json_name")
    );
};
```

## Getters

### Parent Getter

```
// Readonly
//  Returns {"json_name":"string value"}
struct Parent {

    // Use base types (std::string, bool, int, double, etc) when the field should always be rendered
    // Use bound::Json* types for conditional rendering (they have a render property)
    std::string get() {
        return "string value";
    }

    constexpr static auto properties = std::make_tuple(
        bound::property(&Parent::get, "json_name")
    );
};
```

### Child Getter

```
struct Child {

    // When using a casting operator, a bound::Json* type is required
    operator bound::JsonString() {
        return bound::JsonString{"value"};
    }

};

// Readonly
//  Returns {"json_name":"value"}
struct Parent {

    Child child;

    constexpr static auto properties = std::make_tuple(
        bound::property(&Parent::child, "json_name")
    );
};
```

## Setters

### Parent Setter

```
// On read returns {}
// Can be set with: {"json_name":"foo"}
struct Parent {

    // value = "foo"
    // Except bound::JsonRaw, other bound::Json* values are incompatible here
    void set(std::string &value) {
        //..
    }

    constexpr static auto properties = std::make_tuple(
        bound::property(&Parent::set, "json_name")
    );

};
```

### Child Setter

```
struct Child {

    // Except bound::JsonRaw, other bound::Json* values are incompatible here
    Child &operator =(const std::string& value) {
        //...
        return *this;
    }

};

// On read returns {}
// Can be set with: {"json_name":"foo"}
struct Parent {

    // Child's assignment operator will called with "foo"
    Child child;

    constexpr static auto properties = std::make_tuple(
        bound::property(&Parent::child, "json_name")
    );

};
```

## Conditional Writing

Specific values can be filtered out using `bound::WriteConfig`. Alternatively each of the `bound::Json*` types possess a `bool render` field; which when set to `false`, will be skipped.

The `bound::WriteConfig` setters and filters return a reference, so configurations can be chained. 

WriteConfig options: 

* WriteConfig &SetFilename(const std::string &filename)
    * Only read in `bound::ToJsonFile`
* WriteConfig &SetFilename(const std::string &&filename)
    * Only read in `bound::ToJsonFile`
* WriteConfig &SetMaxDecimalPlaces(int max_dec_places)
* WriteConfig &Filter(bool value)
* WriteConfig &Filter(const char *value)
* WriteConfig &Filter(std::string &&value)
* WriteConfig &Filter(long value)
* WriteConfig &Filter(unsigned long value)
* WriteConfig &Filter(double value)
* WriteConfig &FilterZeroNumbers()
* WriteConfig &FilterNullPointers()
* WriteConfig &FilterEmptyStrings()
* WriteConfig &FilterEmptyArrays()
* WriteConfig &FilterEmptyObjects()
* WriteConfig &FilterEmptiesAndZeroes()
* WriteConfig &SetPrefix(std::string &prefix)
* WriteConfig &SetPrefix(std::string &&prefix)