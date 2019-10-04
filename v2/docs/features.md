# Features

## Supported Types

* Arithmetic: `int`, `unsigned`, `long`, `float`, `double`, `bool`, etc
* Sequential Containers: `std::vector`, `std::list`, `std::deque` 
* `std::string`
* `std::map<K, V>`
* `class`/`struct` with `constexpr static std::tuple<...> properties` field

## Property Declaration

### Known Properties

Json properties can be registered in a `constexpr static std::tuple<...> properties` field:

```
struct Foo {
    int bar;

    constexpr static auto properties = std::make_tuple(
        bound::property(&Foo::bar, "bar")
    );
};
```

### Dynamic Properties

#### Known Value Types

If a JSON object has a dynamic set of keys with known value type `T`, use a `std::map<std::string, T> property:

```
struct Foo {
    int bar;
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
struct Foo {
    int bar;
    std::map<std::string, bound::JsonRaw> addl_props;

    constexpr static auto properties = std::make_tuple(
        bound::property(&Foo::bar, "bar"),
        bound::property(&Foo::addl_props)
    );
};
```

`bound::JsonRaw`'s `value` field is a JSON-formatted `std::string` of the parsed value.


## RawJson

Class: `bound::JsonRaw`. `bound::JsonRaw.value` is a `std::string` with a json-formatted string for arbitrary structures. 

### From Parent

```
struct Parent {
    RawJson raw_json;

    constexpr static auto properties = std::make_tuple(
        bound::property(&Parent::raw_json, "json_name")
    );
};
```

### From Child

```
struct Child {

    operator bound::RawJson() {
        return bound::RawJson("{}");
    }

};
```

## Getters

### Parent Getter

```
struct Parent {

    std::string get() {
        return "";
    }

    constexpr static auto properties = std::make_tuple(
        bound::property(&Parent::get, "json_name")
    );
};
```

### Child Getter

```
struct Child {

    operator bound::JsonString() {
        return bound::JsonString{"value"};
    }

};
```

## Setters

### Parent Setter

```
struct Parent {

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

    Child &operator =(const std::string& value) {
        //...
        return *this;
    }

};
```

## Conditional Writing

Specific values can be filtered out using `bound::WriteConfig`. Alternatively each of the `bound::Json*` types possess a `bool render` field; which when set to `false`, will be skipped.

