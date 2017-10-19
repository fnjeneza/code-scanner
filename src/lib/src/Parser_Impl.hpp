#pragma once

#include <optional>
#include <string>
#include <system_error>
#include <vector>

#include <clang-c/Index.h>

#include "code-scanner/Location.hpp"
#include "code-scanner/Params.hpp"
#include "repository.hpp"

namespace code::analyzer {

class Parser_Impl
{
  public:
    Parser_Impl()  = default;
    ~Parser_Impl() = default;

    std::optional<std::error_code>
    initialize(const std::string &             build_uri,
               const std::vector<std::string> &compile_arguments,
               const std::vector<std::string> &flags_to_ignore);

    Location definition(const TextDocumentPositionParams &params);
    Location reference(const TextDocumentPositionParams &params);

  private:
    repository<std::string> m_repository;
};

} // namespace code::analyzer
