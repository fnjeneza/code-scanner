#pragma once

#include "Range.hpp"
#include <string>

namespace code {
namespace analyzer {

using DocumentUri = std::string;

struct Location
{
    DocumentUri uri;
    Range       range;
};

} // namespace analyzer
} // namespace code
