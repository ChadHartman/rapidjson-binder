#ifndef BOUND_WRITER_SCANNER_TESTS_H_
#define BOUND_WRITER_SCANNER_TESTS_H_

#include "tests.h"

namespace bound_writer_scanner_tests_h_
{

struct Bar
{
    std::string s;
    std::vector<int> v;
    int i = 0;

    constexpr static auto properties = std::make_tuple(
        bound::property(&Bar::s, "string"),
        bound::property(&Bar::v, "array"),
        bound::property(&Bar::i, "integer"));
};

class Foo
{
private:
    Bar bar_;

public:
    Bar &bar()
    {
        return bar_;
    }

    void set_bar(Bar &bar)
    {
        bar_ = bar;
    }

    constexpr static auto properties = std::make_tuple(
        bound::property(&Foo::bar_, "bar_property"),
        bound::property(&Foo::bar, "bar"),
        bound::property(&Foo::set_bar, "bar"));
};

TEST_CASE("Scan Object Properties", "[scanner]")
{
    bound::WriteConfig config;
    bound::write::Scanner scanner{config};
    Foo foo;

    SECTION("Unfiltered")
    {
        REQUIRE(9 == scanner.Scan(foo));
    }

    SECTION("Filtered")
    {
        config.FilterEmptiesAndZeroes();
        REQUIRE(0 == scanner.Scan(foo));
    }
}

TEST_CASE("Scan Simple Properties", "[scanner]")
{
    bound::WriteConfig config;
    bound::write::Scanner scanner{config};

    SECTION("Unfiltered")
    {
        REQUIRE(1 == scanner.Scan(0));
        REQUIRE(1 == scanner.Scan(0u));
        REQUIRE(1 == scanner.Scan(0.0));
        REQUIRE(1 == scanner.Scan(false));

        REQUIRE(1 == scanner.Scan(""));
    }

    SECTION("Filtered")
    {
        // Confirm still referenced
        config.Filter(false);
        printf("<FilterEmptiesAndZeroes>\n");
        config.FilterEmptiesAndZeroes();
        printf("</FilterEmptiesAndZeroes>\n");

        REQUIRE(0 == scanner.Scan(0));
        REQUIRE(0 == scanner.Scan(0u));
        REQUIRE(0 == scanner.Scan(0.0));
        printf("%s\n", bound::write::ToJson(config, bound::WriteConfig()).c_str());
        REQUIRE(0 == scanner.Scan(false));
        REQUIRE(0 == scanner.Scan(""));
    }
}

template <template <typename T, typename...> class Container, typename T>
void TestScanArrayUnfiltered(bound::write::Scanner &scanner, Container<T> &container, T item, int populated_size = 3)
{
    REQUIRE(1 == scanner.Scan(container));
    container.push_back(item);
    container.push_back(item);
    REQUIRE(populated_size == scanner.Scan(container));
}

TEST_CASE("Scan Arrays of Fundamentals", "[scanner]")
{
    bound::WriteConfig config;
    bound::write::Scanner scanner{config};
    std::deque<unsigned> deque;
    std::list<bool> list;
    std::vector<int> vector;

    SECTION("Unfiltered")
    {
        TestScanArrayUnfiltered(scanner, deque, 0u);
        TestScanArrayUnfiltered(scanner, list, false);
        TestScanArrayUnfiltered(scanner, vector, 0);
    }

    SECTION("Filtered")
    {
        config.Filter(false);
        config.FilterEmptiesAndZeroes();

        REQUIRE(0 == scanner.Scan(deque));
        REQUIRE(0 == scanner.Scan(list));
        REQUIRE(0 == scanner.Scan(vector));

        deque.push_back(0);
        list.push_back(false);
        vector.push_back(0);

        REQUIRE(0 == scanner.Scan(deque));
        REQUIRE(0 == scanner.Scan(list));
        REQUIRE(0 == scanner.Scan(vector));

        deque.push_back(1);
        list.push_back(true);
        vector.push_back(1);

        // 1 for the array, 1 for the item
        REQUIRE(2 == scanner.Scan(deque));
        REQUIRE(2 == scanner.Scan(list));
        REQUIRE(2 == scanner.Scan(vector));
    }
}

TEST_CASE("Scan Array of Objects", "[scanner]")
{
    bound::WriteConfig config;
    bound::write::Scanner scanner{config};
    std::deque<std::string> deque;
    std::list<Foo> list;

    SECTION("Unfiltered")
    {
        std::string empty = "";
        Foo foo;
        TestScanArrayUnfiltered(scanner, deque, empty);
        TestScanArrayUnfiltered(scanner, list, foo, 19);
    }

    SECTION("Filtered")
    {
        config.FilterEmptiesAndZeroes();

        REQUIRE(0 == scanner.Scan(deque));
        REQUIRE(0 == scanner.Scan(list));

        deque.push_back("");
        Foo foo;
        list.push_back(foo);

        REQUIRE(0 == scanner.Scan(deque));
        REQUIRE(0 == scanner.Scan(list));

        deque.push_back(" ");
        Bar bar;
        // 1
        bar.i = 1;
        // 1
        bar.s = " ";
        // 2
        bar.v.push_back(1);
        foo.set_bar(bar);
        list.push_back(foo);

        // 1 for the array, 1 for the item
        REQUIRE(2 == scanner.Scan(deque));
        // list: 1; foo: 1; 2 * (bar:  1 bar props: 4)
        REQUIRE(12 == scanner.Scan(list));
    }
}

} // namespace bound_writer_scanner_tests_h_

#endif