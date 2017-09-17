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

    unsigned int line      = 0;
    unsigned int character = 0;
};

} // namespace analyzer
} // namespace code
