#pragma once

#include <iostream>
#include <format>
#include <string>

#define AVERAGE_ID_CHAR_LENGTH 60
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
        std::string ids;
        std::string extra;

        // FUTURE: assuming that ids might be inputted as UUIDs (60 chars)
        ids.reserve((AVERAGE_ID_CHAR_LENGTH + 1) * results.found_ids.size() + 1);
        extra.reserve((AVERAGE_ID_CHAR_LENGTH + 1) * results.extra_ids.size() + 1);

        for (auto &id : results.found_ids)
            ids += std::to_string(id) + ",";
        for (auto &id : results.extra_ids)
            extra += std::to_string(id) + ",";

        // remove trailing comma (only if non-empty)
        if (!ids.empty())
            ids.pop_back();
        if (!extra.empty())
            extra.pop_back();

        return os << std::format("{{\"ids\":[{}], \"extra\":[{}]}}", ids, extra);
    }
}