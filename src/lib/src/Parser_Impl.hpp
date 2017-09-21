#pragma once

#include <string>
#include <vector>

#include <clang-c/CXCompilationDatabase.h>
#include <clang-c/Index.h>

#include "code-scanner/Location.hpp"
#include "code-scanner/Params.hpp"
#include "repository.hpp"

namespace code {
namespace analyzer {

class Parser_Impl
{
  public:
    Parser_Impl();
    ~Parser_Impl();

    // Retrieve a cursor from a file/line/column
    CXCursor cursor(const std::string & filename,
                    const unsigned int &line,
                    const unsigned int &column);

    // Retrieve all callers
    std::vector<CXCursor> callers(const CXCursor &cursor) const;

    void initialize(const std::string &             root_uri,
                    const std::vector<std::string> &compile_arguments,
                    const std::vector<std::string> &flags_to_ignore);
    void parse(const std::string &filename);
    std::vector<std::string> get_all_filenames();
    CXCursor locate_definitions(const std::string &filename);

    Location definition(const TextDocumentPositionParams &params);
    Location reference(const TextDocumentPositionParams &params);

  private:
    void source_compile_flags(const CXCompileCommands &compile_commands);
    std::vector<std::string> header_compile_flags();
    void set_flags(const std::string &filename);

    // Fetch all include directories
    void find_all_include_directories();

  private:
    std::vector<std::string> m_flags;
    repository<std::string> m_repository;
    CXTranslationUnit        m_unit;
    CXCompilationDatabase    m_db;
};

} // namespace analyzer
} // namespace code
