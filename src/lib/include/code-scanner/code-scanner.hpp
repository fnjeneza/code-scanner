#pragma once

#include <experimental/optional>
#include <memory>
#include <string>
#include <system_error>
#include <vector>

#include "code-scanner/Location.hpp"

namespace code {
namespace analyzer {

struct Parser_Impl;
struct InitializeParams;
struct TextDocumentPositionParams;
struct ReferenceParams;

class Parser
{
  public:
    Parser();
    ~Parser();
    // The initialize request is sent as the first request from the client to
    // the server
    // @return empty string if no error, error message otherwise
    std::experimental::optional<std::error_code>
             initialize(const InitializeParams &params);
    Location definition(const TextDocumentPositionParams &params);
    Location references(const ReferenceParams &params);

  private:
    // True if the parser is well initialized
    bool                         m_initialized{false};
    std::unique_ptr<Parser_Impl> pimpl;
};

} // namespace analyzer
} // namespace code
