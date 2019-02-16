#include "utils.hpp"
#include <filesystem>

namespace code {
namespace analyzer {
namespace utils {
std::vector<std::string> split(const std::string_view &argument)
{
    std::vector<std::string> ret;
    std::size_t              start = 0;
    while (true)
    {
        std::size_t found = argument.find(' ', start);
        if (found == std::string::npos)
        {
            ret.push_back(std::string(argument.substr(start)));
            break;
        }
        ret.push_back(std::string(argument.substr(start, found - start)));
        start = found + 1;
    }
    return ret;
}

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

Location location(const CXSourceRange &range)
{
    CXFile   file;
    Location _location;
    clang_getSpellingLocation(clang_getRangeStart(range),
                              &file,
                              &_location.range.start.line,
                              &_location.range.start.character,
                              nullptr);

    clang_getSpellingLocation(clang_getRangeEnd(range),
                              nullptr, // we already retrieved the file
                              &_location.range.end.line,
                              &_location.range.end.character,
                              nullptr);
    // filename
    _location.uri =
        std::filesystem::canonical(to_string(clang_getFileName(file)));
    return _location;
}

Location location(const CXCursor &cursor)
{

    if (clang_Cursor_isNull(cursor))
    {
        return Location();
    }

    auto loc   = clang_getCursorLocation(cursor);
    CXFile   _file;
    Location _location;
    clang_getFileLocation(loc,
                          &_file,
                          &_location.range.start.line,
                          &_location.range.start.character,
                          nullptr);
    _location.uri =
        std::filesystem::canonical(to_string(clang_getFileName(_file)));
    _location.range.end.line      = _location.range.start.line;
    // size of the cursor name
    auto size = static_cast<unsigned>(
        to_string(clang_getCursorSpelling(cursor)).size());
    _location.range.end.character = _location.range.start.character + size - 1;
    return _location;
}

bool is_identifier(const CXCursor &cursor)
{
    CXCursorKind kind = clang_getCursorKind(cursor);
    return (CXCursor_CXXMethod == kind || CXCursor_FunctionDecl == kind);
}

bool is_declaration_locate_in_other_file(const CXCursor &cursor)
{
    CXCursor ref = clang_getCanonicalCursor(clang_getCursorReferenced(cursor));
    return location(cursor).uri != location(ref).uri;
}
} // namespace utils
} // namespace analyzer
} // namespace code
