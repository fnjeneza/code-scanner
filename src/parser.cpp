#include "parser.hpp"

namespace code {
namespace analyzer {
Parser::Parser(const std::string &filename)
    : m_filename{filename}
    , m_index{clang_createIndex(1, 1)}
{
    m_unit = clang_parseTranslationUnit(m_index,
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
    CXFile           file     = clang_getFile(m_unit, m_filename.c_str());
    CXSourceLocation location = clang_getLocation(m_unit, file, line, column);
    return clang_getCursor(m_unit, location);
}

CXCursor Parser::reference(const CXCursor &cursor)
{
    return clang_getCursorReferenced(cursor);
}

CXCursor Parser::definition(const CXCursor &cursor)
{
    return clang_getCursorDefinition(cursor);
}

std::vector<CXCursor> Parser::callers(const CXCursor &cursor)
{
    // get cursor declaration/definition
    CXCursor cursor_decl = definition(cursor);

    // get translation unit cursor
    CXCursor unit_cursor = clang_getTranslationUnitCursor(m_unit);

    // traverse the AST and check every cursor if it is equal to the
    // cursor declaration
    // ignore the cursor which point to himself
    CXCursor     cursor_temp = const_cast<CXCursor &>(cursor);
    CXClientData user_data   = static_cast<CXClientData>(&cursor_temp);
    clang_visitChildren(
        unit_cursor,
        // visitor
        [](CXCursor cursor, CXCursor, CXClientData client_data) {
            // check the current cursor and AST cursor
            CXCursor *ast_cursor = static_cast<CXCursor *>(client_data);
            // get cursor declaration
            // TODO
            unsigned is_equal = clang_equalCursors(cursor, *ast_cursor);
            if (is_equal)
            {
                // TODO process here
            }
            return CXChildVisit_Recurse;
        },
        user_data);

    std::vector<CXCursor> cursors;
    return cursors;
}
// Retrieve a type of cursor
std::string Parser::type(const CXCursor &cursor)
{
    // cursor type
    CXType type = clang_getCursorType(cursor);
    // cursor type spelling
    CXString spelling = clang_getTypeSpelling(type);
    // cursor spelling as string
    std::string spelling_str = clang_getCString(spelling);
    // freed the CXString
    clang_disposeString(spelling);
    return spelling_str;
}

std::tuple<std::string, unsigned long, unsigned long>
Parser::location(const CXCursor &cursor)
{
    CXSourceLocation location = clang_getCursorLocation(cursor);

    CXFile   file;
    unsigned line;
    unsigned column;
    clang_getSpellingLocation(location, &file, &line, &column, nullptr);

    // filename
    CXString    _filename = clang_getFileName(file);
    std::string filename  = clang_getCString(_filename);
    clang_disposeString(_filename);

    return std::make_tuple(filename, line, column);
}

std::string Parser::filename()
{
    CXFile      file     = clang_getFile(m_unit, m_filename.c_str());
    CXString    spelling = clang_getFileName(file);
    std::string name     = clang_getCString(spelling);
    clang_disposeString(spelling);
    return name;
}

} // namespace analyzer
} // namespace code
