#pragma once

#include <memory>
#include <set>
#include <string>

#include "code-scanner/Location.hpp"
#include "code-scanner/Position.hpp"

struct CXTranslationUnitImpl;
using CXTranslationUnit = CXTranslationUnitImpl *;

namespace code {
namespace analyzer {

enum class translation_unit_flag
{
    none                 = 0x0,
    skip_function_bodies = 0x40,
};

class translation_unit_t
{
  public:
    translation_unit_t(const std::string &filename,
                       const bool         skip_function_bodies = false);
    ~translation_unit_t();

    Location definition(const Position &position) const;
    Location definition(const std::string &usr) const;
    Location reference(const Position &position) const;

    std::string usr(const Position &position) const;

    std::set<std::string> retrieve_all_identifier_usr() const;

  private:
    void
    parse(const translation_unit_flag &option = translation_unit_flag::none);

  private:
    CXTranslationUnit m_unit;
    std::string       m_filename;
};
} // namespace analyzer
} // namespace code
