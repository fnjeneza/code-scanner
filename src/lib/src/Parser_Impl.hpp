#pragma once

#include <experimental/optional>
#include <memory>
#include <mutex>
#include <shared_mutex>
#include <string>
#include <system_error>
#include <vector>

#include "code-scanner/Location.hpp"
#include "code-scanner/Params.hpp"
#include "file.hpp"
#include "repository.hpp"
#include "translation_unit_t.hpp"

namespace code::analyzer {
struct compile_database_t;

class Parser_Impl
{
  public:
    Parser_Impl();
    ~Parser_Impl();

    std::experimental::optional<std::error_code>
    initialize(const std::string &             build_uri,
               const std::vector<std::string> &compile_arguments,
               const std::vector<std::string> &flags_to_ignore);

    Location definition(const TextDocumentPositionParams &params) const
        noexcept;
    Location reference(const TextDocumentPositionParams &params) const noexcept;

  private:
    Location find(const TextDocumentPositionParams &params,
                  const kind &                      predicate) const noexcept;

  private:
    // repository<std::string>             m_repository;
    translation_unit_t                  m_tu;
    std::unique_ptr<compile_database_t> m_compile_db;
    // TODO use a database to store symbols
    std::set<symbol>           m_index;
    std::set<utils::File>      m_indexed_files;
    std::mutex                 m_mutex;
};

} // namespace code::analyzer
