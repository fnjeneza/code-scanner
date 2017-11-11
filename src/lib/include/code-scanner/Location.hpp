#pragma once

#include "Range.hpp"
#include <string>

namespace code {
namespace analyzer {

using DocumentUri = std::string;

struct Location
{
    Location()  = default;
    ~Location() = default;

    inline bool is_valid() { return !uri.empty(); }
    friend bool operator==(const Location &lhs, const Location &rhs)
    {
        return lhs.uri == rhs.uri && lhs.range == rhs.range;
    }

    DocumentUri uri;
    Range       range;
};

} // namespace analyzer
} // namespace code
