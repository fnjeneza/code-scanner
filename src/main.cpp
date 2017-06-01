#include <clang-c/Index.h>
#include <iostream>
#include <string>

std::ostream& operator<<(std::ostream& stream, const CXString& cx_string)
{
    stream << clang_getCString(cx_string);
    clang_disposeString(cx_string);
    return stream;
}

// Retrieve a type of cursor
std::string type(const CXCursor &cursor)
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

int main(int argc, char **argv)
{
    CXIndex index = clang_createIndex(0,0);
    CXTranslationUnit unit = clang_parseTranslationUnit(
            index,
            argv[1],
            nullptr,
            0,
            nullptr,
            0,
            CXTranslationUnit_None);
    CXFile file = clang_getFile(unit, argv[1]);
    std::cout << clang_getFileName(file) << std::endl;

    unsigned line = std::stoi(argv[2]);
    unsigned column = std::stoi(argv[3]);
    CXSourceLocation loc = clang_getLocation(unit, file, line, column);
    CXCursor c = clang_getCursor(unit, loc);
    std::cout << "Cursor kind: " << clang_getCursorDisplayName(c)
    << "\t"<< clang_getCursorKindSpelling(clang_getCursorKind(c)) <<'\n';
    //CXType type = clang_getCursorType(c);
    //std::cout << "\n" << clang_getTypeSpelling(type) << std::endl;
    std::cout << "\n" << type(c) << std::endl;
    c = clang_getCursorSemanticParent(c);
    std::cout << "Cursor kind: " << clang_getCursorDisplayName(c)
    << "\t"<< clang_getCursorKindSpelling(clang_getCursorKind(c)) <<'\n';


/*
    if(unit == nullptr)
    {
        std::cout << "Unable to parse" << std::endl;
    }

    CXCursor cursor = clang_getTranslationUnitCursor(unit);
    clang_visitChildren(
            cursor,
            [](CXCursor x, CXCursor parent, CXClientData client_data)
            {
                CXCursor c = clang_getCanonicalCursor(x);
                std::cout << "Cursor kind: " << clang_getCursorDisplayName(c)
                << "\t"<< clang_getCursorKindSpelling(clang_getCursorKind(c)) <<'\n';
                return CXChildVisit_Recurse;
            },
            nullptr);

*/
    clang_disposeTranslationUnit(unit);
    clang_disposeIndex(index);
}
