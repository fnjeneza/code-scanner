#pragma once

#include "Position.hpp"

namespace code {
namespace analyzer {

struct Range
{
    Range()  = default;
    ~Range() = default;

    Range(const Position &_start, const Position &_end)
    {
        start = _start;
        end   = _end;
    }

    friend bool operator==(const Range &lhs, const Range &rhs)
    {
        return lhs.start == rhs.start && lhs.end == rhs.end;
    }

    Position start;
    Position end;
};

} // namespace analyzer
} // namespace code
