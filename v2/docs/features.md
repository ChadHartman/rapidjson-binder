# Features

## Supported Types

* Arithmetic: `int`, `unsigned`, `long`, `float`, `double`, `bool`, etc
* Sequential Containers: `std::vector`, `std::list`, `std::deque` 
* `std::string`
* `std::map<std::string, ?>`
* `class`/`struct` with `constexpr static std::tuple<...> properties` field

## Property Declaration

### Known Properties

Json properties can be registered in a `constexpr static std::tuple<...> properties` field.  

```
struct Parent {
    int property;

    constexpr static auto properties = std::make_tuple(
        bound::property(&Parent::property, "json_name")
    );
};
```

### Dynamic Properties

If a JSON object has a dynamic set of keys with an unknown TODO

Use `std::map<std::string, bound::RawJson>` or if the value type is known: `std::map<std::string, Child>`.


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

