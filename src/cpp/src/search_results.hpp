#pragma once

#include <iostream>
#include <format>

namespace QuadTree
{
    struct SearchResults
    {
        /** Ids that were found within the Radius */
        std::vector<int> found_ids;
        /** Ids that were found within the 2*Radius, excluding the  */
        std::vector<int> extra_ids;
    };

    inline std::ostream &operator<<(std::ostream &os, const class SearchResults &results)
    {
        return os
               << std::format("<TODO: SearchResults; {}, {}>", results.found_ids.size(), results.extra_ids.size());
    }
}