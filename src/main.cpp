#include "parser.hpp"
#include <iostream>

std::ostream &operator<<(std::ostream &stream, const CXString &cx_string)
{
    stream << clang_getCString(cx_string);
    clang_disposeString(cx_string);
    return stream;
}

int main(int argc, char **argv)
{
    code::analyzer::Parser parser(argv[1]);
    unsigned long          line   = std::stoul(argv[2]);
    unsigned long          column = std::stoul(argv[3]);
    CXCursor               cursor = parser.cursor(line, column);
    cursor                        = code::analyzer::definition(cursor);

    auto cursors = parser.callers(cursor);
    for (auto cursor : cursors)
    {
        // retrieve location
        auto loc = code::analyzer::location(cursor);
        std::cout << "Defintion :" << std::endl;
        std::cout << std::get<0>(loc) << std::endl;
        std::cout << std::get<1>(loc) << std::endl;
        std::cout << std::get<2>(loc) << std::endl;
    }

    std::cout << "Cursor kind:\t" << clang_getCursorDisplayName(cursor) << "\t"
              << clang_getCursorKindSpelling(clang_getCursorKind(cursor))
              << '\n';
    std::cout << "type: " << code::analyzer::type(cursor) << std::endl;
    cursor = clang_getCursorReferenced(cursor);
    std::cout << "cursor referenced kind:\t"
              << clang_getCursorDisplayName(cursor) << "\t"
              << clang_getCursorKindSpelling(clang_getCursorKind(cursor))
              << '\n';
}
