# Features

## Supported Types

* Arithmetic: `int`, `unsigned`, `long`, `float`, `double`, `bool`, etc
* Sequential Containers: `std::vector`, `std::list`, `std::deque` 
* `std::string`
* `std::map<std::string, ?>`

## Property Declaration

```
struct Parent {
    int property;

    constexpr static auto properties = std::make_tuple(
        bound::property(&Parent::property, "json_name")
    );
};
```

## RawJson

Uses `bound::RawJson`

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

    operator std::string() {
        return "";
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

If an outputted property shouldn't render, have a child/parent get return a `NULL` pointer.

## Dynamic Key Properties

Properties with unknown keys.

Use `std::map<std::string, bound::RawJson>` or if the value type is known: `std::map<std::string, Child>`.