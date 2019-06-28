#ifndef BOUND_WRITER_SCANNER_TESTS_H_
#define BOUND_WRITER_SCANNER_TESTS_H_

#include "tests.h"

namespace scanner_tests
{

TEST_CASE("Scan Object Properties", "[scanner]")
{
    bound::WriteConfig config;
    bound::writer::Scanner scanner{config};
    Foo foo;

    SECTION("Unfiltered")
    {
        // 1 for foo, 2 * (1 for bar, and 3 bar child properties)
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
    bound::writer::Scanner scanner{config};

    SECTION("Unfiltered")
    {
        REQUIRE(1 == scanner.Scan(0));
        REQUIRE(1 == scanner.Scan(0u));
        REQUIRE(1 == scanner.Scan(0.0));
        REQUIRE(1 == scanner.Scan(false));

        std::string src{""};
        REQUIRE(1 == scanner.Scan(src));

        std::string &ref = src;
        REQUIRE(1 == scanner.Scan(ref));
    }

    SECTION("Filtered")
    {
        // Confirm still referenced
        config.Filter(false);
        config.FilterEmptiesAndZeroes();

        REQUIRE(0 == scanner.Scan(0));
        REQUIRE(0 == scanner.Scan(0u));
        REQUIRE(0 == scanner.Scan(0.0));
        REQUIRE(0 == scanner.Scan(false));

        std::string src{""};
        REQUIRE(0 == scanner.Scan(src));

        std::string &ref = src;
        REQUIRE(0 == scanner.Scan(ref));
    }
}

template <template <typename T, typename...> class Container, typename T>
void TestScanArrayUnfiltered(bound::writer::Scanner &scanner, Container<T> &container, T item)
{
    REQUIRE(1 == scanner.Scan(container));
    container.push_back(item);
    container.push_back(item);
    REQUIRE(3 == scanner.Scan(container));
}

TEST_CASE("Scan Arrays of Fundamentals", "[scanner]")
{
    bound::WriteConfig config;
    bound::writer::Scanner scanner{config};
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
    bound::writer::Scanner scanner{config};
    std::deque<std::string> deque;
    std::list<Foo> list;

    SECTION("Unfiltered")
    {
        std::string empty = "";
        Foo foo;
        TestScanArrayUnfiltered(scanner, deque, empty);
        TestScanArrayUnfiltered(scanner, list, foo);
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

} // namespace scanner_tests

#endif