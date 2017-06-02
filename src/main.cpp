#include <parser.hpp>
#include <iostream>

std::ostream& operator<<(std::ostream& stream, const CXString& cx_string)
{
    stream << clang_getCString(cx_string);
    clang_disposeString(cx_string);
    return stream;
}

int main(int argc, char **argv)
{
    code::analyzer::Parser parser(argv[1]);
    unsigned long line  = std::stoul(argv[2]);
    unsigned long column = std::stoul(argv[3]);
    CXCursor cursor = parser.cursor(line, column);

    std::cout << "Cursor kind:\t" << clang_getCursorDisplayName(cursor)
    << "\t"<< clang_getCursorKindSpelling(clang_getCursorKind(cursor)) <<'\n';
    std::cout << "type: " << parser.type(cursor) << std::endl;
    cursor = clang_getCursorSemanticParent(cursor);
    std::cout << "Parent cursor kind:\t" << clang_getCursorDisplayName(cursor)
    << "\t"<< clang_getCursorKindSpelling(clang_getCursorKind(cursor)) <<'\n';
}
