#pragma once

#include <clang-c/Index.h>
#include <string>
#include <tuple>

namespace code {
namespace analyzer {

class Parser
{
  public:
    Parser(const std::string &filename);
    ~Parser();

    // Retrieve a cursor from a file/line/column
    CXCursor cursor(const unsigned long &line, const unsigned long &column);

    // Retrieve the reference of a cursor
    CXCursor reference(const CXCursor &cursor);

    CXCursor definition(const CXCursor &cursor);

    // Retrieve a type of cursor
    std::string type(const CXCursor &cursor);

    std::tuple<std::string, unsigned long, unsigned long>
    location(const CXCursor &cursor);

    //.Retrieve name of the file being processed
    std::string filename();

  private:
    std::string       m_filename;
    CXIndex           m_index;
    CXTranslationUnit m_unit;
};

} // namespace analyzer
} // namespace code
