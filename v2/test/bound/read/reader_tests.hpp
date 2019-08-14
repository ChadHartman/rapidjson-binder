#ifndef BOUND_READ_READER_TESTS_HPP_
#define BOUND_READ_READER_TESTS_HPP_

namespace bound_read_reader_tests_hpp_
{

struct Foo
{
    constexpr static auto BOUND_PROPS_NAME = std::make_tuple();
};

TEST_CASE("Reader Tests", "[reader_tests]")
{
    Foo foo = bound::read::FromJson<Foo>("{}");
}

} // namespace bound_read_reader_tests_hpp_

#endif
