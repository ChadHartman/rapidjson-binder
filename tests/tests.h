#ifndef TESTS_H_
#define TESTS_H_


#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"
#include "bound/bound.h"

namespace test
{

template <typename T>
typename std::enable_if<std::is_floating_point<T>::value, bool>::type
fp_equals(T a, T b)
{
    return std::abs(a - b) < std::numeric_limits<T>::epsilon();
}

} // namespace test

#include "test/foo_bar.h"
#include "test/prop_class.h"

#endif