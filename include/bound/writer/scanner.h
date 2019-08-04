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

#ifndef BOUND_WRITER_SCANNER_H_
#define BOUND_WRITER_SCANNER_H_

#include "../write_config.h"
#include "authorizer.h"

namespace bound
{
namespace writer
{

// Detects whether a JSON Property needs to be written
class Scanner
{
private:
    const WriteConfig &write_config_;

    // Standard Property
    template <typename I, typename M>
    typename std::enable_if_t<std::is_member_object_pointer<M>::value, unsigned int>
    ScanProperty(I &instance, const char *key, M member)
    {
        return Scan(instance.*(member));
    }

    // Getter
    template <typename I, typename M>
    typename std::enable_if_t<util::is_getter<M>::value, unsigned int>
    ScanProperty(I &instance, const char *key, M member)
    {
        auto value = (instance.*(member))();
        return Scan(value);
    }

    // Setter
    template <typename I, typename M>
    typename std::enable_if_t<util::is_setter<M>::value || util::is_authorizer<M>::value, unsigned int>
    ScanProperty(I &instance, const char *key, M member)
    {
        // Do nothing
        return 0;
    }

public:
    Scanner(const WriteConfig &write_config) : write_config_{write_config} {}

    template <typename T>
    typename std::enable_if<std::is_pointer<T>::value, unsigned int>::type
    Scan(T value)
    {
        return Scan(*value);
    }

    unsigned int Scan(DynamicProperties &json_properties)
    {
        return json_properties.size();
    }

    // Scan Object
    // Returns count of printable properties
    template <typename T>
    typename std::enable_if<
        !util::is_simple_property<T>::value &&
            !util::is_seq_container<T>::value &&
            !std::is_pointer<T>::value,
        unsigned int>::type
    Scan(T &instance)
    {
        unsigned int count = 0;

        Authorizer<T> authorizer{instance};

        constexpr auto prop_count = std::tuple_size<decltype(T::BOUND_PROPS_NAME)>::value;

        util::for_sequence(std::make_index_sequence<prop_count>{}, [&](auto i) {
            constexpr auto property = std::get<i>(T::BOUND_PROPS_NAME);
            if (!authorizer.Authorize(property.name))
            {
                return;
            }

            count += ScanProperty(instance, property.name, property.member);
        });

        if (!write_config_.IsFilteringEmptyObjects())
        {
            // Return 1 for self when not filtering
            return count + 1;
        }

        // 1 for this object when it was properties
        return count == 0 ? 0 : count + 1;
    }

    // Scan Simple Field
    // Returns count of printable properties
    template <typename T>
    typename std::enable_if<
        util::is_simple_property<T>::value &&
            !std::is_pointer<T>::value,
        unsigned int>::type
    Scan(const T value)
    {
        unsigned int count = write_config_.IsFiltered(value) ? 0 : 1;
        return count;
    }

    // Scan Array of primatives
    // Returns count of printable properties
    template <template <typename...> class Container, typename T>
    typename std::enable_if<
        util::is_seq_container<Container<T>>::value &&
            std::is_fundamental<T>::value,
        unsigned int>::type
    Scan(Container<T> &array)
    {
        if (!write_config_.IsFilteringEmptyArrays())
        {
            // Return 1 for self when not filtering
            return array.size() + 1;
        }

        unsigned int count = 0;
        for (T value : array)
        {
            count += Scan(value);
        }

        return count == 0 ? 0 : count + 1;
    }

    // Scan Array of c objects
    // Returns count of printable properties
    template <template <typename...> class Container, typename T>
    typename std::enable_if<
        util::is_seq_container<Container<T>>::value &&
            !std::is_fundamental<T>::value,
        unsigned int>::type
    Scan(Container<T> &array)
    {
        if (!write_config_.IsFilteringEmptyArrays())
        {
            // Return 1 for self when not filtering
            return array.size() + 1;
        }

        unsigned int count = 0;
        for (T &value : array)
        {
            count += Scan(value);
        }

        // Filtering; return 1 for self, only if there's more than 1 item in container
        return count == 0 ? 0 : count + 1;
    }
};

} // namespace writer

} // namespace bound

#endif