#pragma once

#include <string>
#include <concepts>

namespace QuadTree
{
    /** Convert an integral (number) into a comma (change via `sep`) separated string. */
    template <typename T>
        requires std::integral<T> || std::floating_point<T>
    std::string commaify(T value, char sep = ',')
    {
        auto s = std::to_string(value);

        auto pos = static_cast<std::ptrdiff_t>(s.find_first_not_of('-'));

        auto decimal_pos = s.find('.');
        if (decimal_pos == std::string::npos)
            decimal_pos = s.size();

        for (auto i = static_cast<std::ptrdiff_t>(decimal_pos) - 3; i > pos; i -= 3)
        {
            s.insert(i, 1, sep);
        }

        return s;
    }
}