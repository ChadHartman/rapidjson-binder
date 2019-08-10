/*
Copyright 2019 Chad Hartman

Permission is hereby granted, free of charge, to any person obtaining a copy of 
this software and associated documentation files (the "Software"), to deal in the 
Software without restriction, including without limitation the rights to use, copy, 
modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, 
and to permit persons to whom the Software is furnished to do so, subject to the 
following conditions:

The above copyright notice and this permission notice shall be included in all 
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, 
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT 
HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION 
OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE 
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. 
*/

#ifndef BOUND_WRITE_CONFIG_H_
#define BOUND_WRITE_CONFIG_H_

//#define BOUND_WRITE_CONFIG_H_DEBUG

#include <string>
#include <cmath>

#include "property.h"
#include "type_traits.h"

namespace bound
{

// Set of rules to determine how JSON is written
class WriteConfig
{
private:
    template <typename T>
    struct PropertyFilter
    {
        bool enabled;
        T value;
        using type = T;

        constexpr static auto properties = std::make_tuple(
            property(&PropertyFilter::enabled, "enabled"),
            property(&PropertyFilter::value, "value"));
    };

    PropertyFilter<bool> bool_filter_{false, false};
    // uint 64 because that's what the writer uses
    PropertyFilter<unsigned long long> uint_filter_{false, 0u};
    // int64 because that's what the writer uses
    PropertyFilter<long long> int_filter_{false, 0};
    PropertyFilter<double> float_filter_{false, 0.0};
    PropertyFilter<std::string> string_filter_{false, ""};

    bool filter_null_pointers_ = false;
    bool filter_empty_arrays_ = false;
    bool filter_empty_objects_ = false;

public:
    bool IsFilteringNullPointers() const
    {
        return filter_null_pointers_;
    }

    bool IsFilteringEmptyArrays() const
    {
        return filter_empty_arrays_;
    }

    bool IsFilteringEmptyObjects() const
    {
        return filter_empty_objects_;
    }

    bool IsFiltered(bool value) const
    {
        return bool_filter_.enabled && bool_filter_.value == value;
    }

    template <typename T>
    typename std::enable_if<is_int<T>::value, bool>::type
    IsFiltered(T value) const
    {
        return int_filter_.enabled && int_filter_.value == value;
    }

    template <typename T>
    typename std::enable_if<is_uint<T>::value, bool>::type
    IsFiltered(T value) const
    {
        return uint_filter_.enabled && uint_filter_.value == value;
    }

    template <typename T>
    typename std::enable_if<std::is_floating_point<T>::value, bool>::type
    IsFiltered(T value) const
    {
        return float_filter_.enabled &&
               fabs(value - float_filter_.value) < std::numeric_limits<T>::epsilon();
    }

    template <typename T>
    bool IsFiltered(std::string value) const
    {
        return string_filter_.enabled && string_filter_.value == value;
    }

    WriteConfig &Filter(bool value)
    {
        bool_filter_.enabled = true;
        bool_filter_.value = value;
        return *this;
    }

    WriteConfig &Filter(std::string &&value)
    {
        string_filter_.enabled = true;
        string_filter_.value = value;
        return *this;
    }

    template <typename T>
    typename std::enable_if<is_int<T>::value, WriteConfig &>::type
    Filter(T value)
    {
        int_filter_.enabled = true;
        int_filter_.value = static_cast<decltype(int_filter_)::type>(value);
        return *this;
    }

    template <typename T>
    typename std::enable_if<is_uint<T>::value, WriteConfig &>::type
    Filter(T value)
    {
        uint_filter_.enabled = true;
        uint_filter_.value = static_cast<decltype(uint_filter_)::type>(value);
        return *this;
    }

    template <typename T>
    typename std::enable_if<std::is_floating_point<T>::value, WriteConfig &>::type
    Filter(T value)
    {
        float_filter_.enabled = true;
        float_filter_.value = static_cast<decltype(float_filter_)::type>(value);
        return *this;
    }

    WriteConfig &FilterZeroNumbers()
    {
        Filter(0);
        Filter(0u);
        Filter(0.0);
        return *this;
    }

    WriteConfig &FilterNullPointers()
    {
        filter_null_pointers_ = true;
        return *this;
    }

    WriteConfig &FilterEmptyStrings()
    {
        Filter("");
        return *this;
    }

    WriteConfig &FilterEmptyArrays()
    {
        filter_empty_arrays_ = true;
        return *this;
    }

    WriteConfig &FilterEmptyObjects()
    {
        filter_empty_objects_ = true;
        return *this;
    }

    WriteConfig &FilterEmptiesAndZeroes()
    {
        FilterZeroNumbers();
        FilterEmptyArrays();
        FilterEmptyObjects();
        FilterEmptyStrings();
        return *this;
    }

    constexpr static auto properties = std::make_tuple(
        property(&WriteConfig::bool_filter_, "bool_filter"),
        property(&WriteConfig::uint_filter_, "uint_filter"),
        property(&WriteConfig::int_filter_, "int_filter"),
        property(&WriteConfig::float_filter_, "float_filter"),
        property(&WriteConfig::string_filter_, "string_filter"),
        property(&WriteConfig::filter_null_pointers_, "filter_null_pointers"),
        property(&WriteConfig::filter_empty_arrays_, "filter_empty_arrays_flag"),
        property(&WriteConfig::filter_empty_objects_, "filter_empty_objects_flag"));
};

} // namespace bound

#endif