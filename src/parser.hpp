#pragma once

#include <clang-c/Index.h>
#include <string>
#include <tuple>
#include <vector>

namespace code {
namespace analyzer {

class Parser
{
  public:
    Parser(const std::string &filename);
    ~Parser();

    // Retrieve a cursor from a file/line/column
    CXCursor cursor(const unsigned long &line, const unsigned long &column);

    // Retrieve all callers
    std::vector<CXCursor> callers(const CXCursor &cursor) const;

    //.Retrieve name of the file being processed
    std::string filename() const;

  private:
    std::string       m_filename;
    CXIndex           m_index;
    CXTranslationUnit m_unit;
};

// Retrieve the reference of a cursor
CXCursor reference(const CXCursor &cursor);

// Retrieve the declaration of a cursor
CXCursor declaration(const CXCursor &cursor);

// Retrieve a type of cursor
std::string type(const CXCursor &cursor);

// Retrieve the location as file, line, column
std::tuple<std::string, unsigned long, unsigned long>
location(const CXCursor &cursor);
} // namespace analyzer
} // namespace code
