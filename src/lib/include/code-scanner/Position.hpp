#pragma once

namespace code {
namespace analyzer {

struct Position
{
    Position()  = default;
    ~Position() = default;

    Position(const unsigned &_line, const unsigned &_character)
    {
        line      = _line;
        character = _character;
    }

    friend bool operator==(const Position &lhs, const Position &rhs)
    {
        return lhs.line == rhs.line && lhs.character == rhs.character;
    }

    unsigned int line      = 0;
    unsigned int character = 0;
};

} // namespace analyzer
} // namespace code
