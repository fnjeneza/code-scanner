#pragma once

#include <experimental/optional>
#include <string>
#include <system_error>
#include <vector>

#include "code-scanner/Location.hpp"
#include "code-scanner/Params.hpp"
#include "repository.hpp"
#include "translation_unit_t.hpp"

namespace code::analyzer {

class Parser_Impl
{
  public:
    Parser_Impl()  = default;
    ~Parser_Impl() = default;

    std::experimental::optional<std::error_code>
    initialize(const std::string &             build_uri,
               const std::vector<std::string> &compile_arguments,
               const std::vector<std::string> &flags_to_ignore);

    Location definition(const TextDocumentPositionParams &params);
    Location reference(const TextDocumentPositionParams &params);

  private:
    repository<std::string> m_repository;
    translation_unit_t      m_tu;
};

} // namespace code::analyzer
