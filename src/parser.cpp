#include "parser.hpp"

namespace code{
namespace analyzer{
Parser::Parser(const std::string &filename)
    : m_filename{filename}
      , m_index{clang_createIndex(1,1)}
{
    m_index = clang_createIndex(0,0);
    m_unit = clang_parseTranslationUnit(
            m_index,
            m_filename.c_str(),
            nullptr,
            0,
            nullptr,
            0,
            CXTranslationUnit_None);
}

Parser::~Parser()
{
    clang_disposeTranslationUnit(m_unit);
    clang_disposeIndex(m_index);
}


// Retrieve a cursor from a file/line/column
CXCursor Parser::cursor(const unsigned long &line, const unsigned long &column)
{
    CXFile file = clang_getFile(m_unit, m_filename.c_str());
    CXSourceLocation location = clang_getLocation(m_unit, file, line, column);
    return clang_getCursor(m_unit, location);
}


// Retrieve a type of cursor
std::string Parser::type(const CXCursor &cursor)
{
    //cursor type
    CXType type = clang_getCursorType(cursor);
    // cursor type spelling
    CXString spelling = clang_getTypeSpelling(type);
    // cursor spelling as string
    std::string spelling_str = clang_getCString(spelling);
    // freed the CXString
    clang_disposeString(spelling);
    return spelling_str;
}

std::tuple<std::string, unsigned long, unsigned long> Parser::location(const CXCursor &cursor)
{
    return std::make_tuple("",0,0);
}

std::string Parser::filename()
{
    CXFile file = clang_getFile(m_unit, m_filename.c_str());
    CXString spelling = clang_getFileName(file);
    std::string name = clang_getCString(spelling);
    clang_disposeString(spelling);
    return name;
}

} // namespace analyzer
} // namespace code
