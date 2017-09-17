#pragma once

#include <clang-c/Index.h>
#include <set>
#include <string>

namespace code {
namespace analyzer {
class translation_unit_t
{
  public:
    translation_unit_t(const std::string &filename);
    ~translation_unit_t();

    std::set<std::string> retrieve_all_identifier_usr();

  private:
    void parse();

  private:
    CXTranslationUnit m_unit;
    std::string       m_filename;
};
}
}
