#include "functional.hpp"
#include <iostream>
#include <clang-c/CXCompilationDatabase.h>

#include <tuple>

namespace code {
namespace analyzer {

std::string to_string(const CXString &cx_str)
{
    auto cstr = clang_getCString(cx_str);

    if (cstr == NULL)
    {
        return std::string();
    }

    std::string str(cstr);
    clang_disposeString(cx_str);
    return str;
}

CXCursor declaration(const CXCursor &cursor)
{
    auto cur = clang_getCursorDefinition(cursor);
    return clang_getCanonicalCursor(cur);
}

CXCursor reference(const CXCursor &cursor)
{
    return clang_getCursorReferenced(cursor);
}

CXCursor definition(const CXCursor &cursor)
{
    // search for definition in current translation unit
    CXCursor cursor_ = clang_getCursorDefinition(cursor);
    return cursor_;
}

// Retrieve a type of cursor
std::string type(const CXCursor &cursor)
{
    // cursor type
    CXType type = clang_getCursorType(cursor);
    // cursor type spelling
    std::string spelling = to_string(clang_getTypeSpelling(type));
    return spelling;
}

std::tuple<std::string, unsigned int, unsigned int>
location(const CXCursor &cursor)
{

    CXSourceLocation location = clang_getCursorLocation(cursor);

    CXFile   file;
    unsigned line   = 0;
    unsigned column = 0;
    clang_getSpellingLocation(location, &file, &line, &column, nullptr);

    // filename
    std::string _filename = to_string(clang_getFileName(file));
    return std::make_tuple(_filename, line, column);
}

Location get_location(const CXCursor &cursor)
{

    if (clang_Cursor_isNull(cursor))
    {
        return Location();
    }

    CXSourceLocation location = clang_getCursorLocation(cursor);

    CXFile   file;
    unsigned line   = 0;
    unsigned column = 0;
    clang_getSpellingLocation(location, &file, &line, &column, nullptr);

    // filename
    std::string _filename = to_string(clang_getFileName(file));
    return Location(_filename, line, column);
}

bool is_identifier(CXCursor &cursor)
{
    CXCursorKind kind = clang_getCursorKind(cursor);
    return (CXCursor_CXXMethod == kind || CXCursor_FunctionDecl == kind);
}

bool is_declaration_locate_in_other_file(CXCursor &cursor)
{
    CXCursor ref = clang_getCanonicalCursor(clang_getCursorReferenced(cursor));
    return std::get<0>(location(cursor)) != std::get<0>(location(ref));
}

} // namespace analyzer
} // namespace code
