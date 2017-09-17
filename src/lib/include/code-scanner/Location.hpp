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

    Location(const DocumentUri &_uri,
             const unsigned &   _line,
             const unsigned &   _column)
    {
        uri   = _uri;
        range = Range(Position(_line, _column), Position(_line, _column));
    }

    DocumentUri uri;
    Range       range;
};

} // namespace analyzer
} // namespace code
