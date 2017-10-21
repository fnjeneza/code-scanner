#include "utils.hpp"

namespace code {
namespace analyzer {
namespace utils {
std::vector<std::string> split(const std::string &argument)
{
    std::vector<std::string> ret;
    std::size_t              start = 0;
    while (true)
    {
        std::size_t found = argument.find(' ', start);
        if (found == std::string::npos)
        {
            ret.push_back(argument.substr(start));
            break;
        }
        ret.push_back(argument.substr(start, found - start));
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

Location location(const CXCursor &cursor)
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
    return location(cursor).uri != location(ref).uri;
}
} // namespace utils
} // namespace analyzer
} // namespace code
