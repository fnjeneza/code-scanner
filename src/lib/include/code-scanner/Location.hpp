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

    DocumentUri uri;
    Range       range;
};

} // namespace analyzer
} // namespace code
