#include <clang-c/Index.h>
#include <iostream>
#include <string>

std::ostream& operator<<(std::ostream& stream, const CXString& cx_string)
{
    stream << clang_getCString(cx_string);
    clang_disposeString(cx_string);
    return stream;
}


int main(int argc, char **argv)
{
    CXIndex index = clang_createIndex(1,1);
    CXTranslationUnit unit = clang_parseTranslationUnit(
            index,
            argv[1],
            nullptr,
            0,
            nullptr,
            0,
            CXTranslationUnit_None);

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


    clang_disposeTranslationUnit(unit);
    clang_disposeIndex(index);
}
