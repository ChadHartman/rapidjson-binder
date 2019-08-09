#ifndef BOUND_WRITE_WRITER_H_
#define BOUND_WRITE_WRITER_H_

#include <string>

#include "../property_iterator.h"
#include "../type_traits.h"

namespace bound
{

namespace write
{

template <typename O>
//typename std::enable_if<std::is_same<decltype(O::BOUND_PROPS_NAME), std::tuple<T...>>::value, std::string>::type
typename std::enable_if<is_bound<O>::value, std::string>::type
Write(O &object)
{
    ListProperties(object, [&](auto property) {
        printf("%s\n", property.name);
    });

    return "";
}

template <typename O>
std::string Write(O &object)
{
    printf("Object has no properties\n");

    return "";
}

} // namespace write

} // namespace bound

#endif