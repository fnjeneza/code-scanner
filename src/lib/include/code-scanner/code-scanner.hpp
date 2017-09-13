#pragma once

#include <memory>
#include <string>
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
    void initialize(const InitializeParams &params);
    Location definition(const TextDocumentPositionParams &params);
    Location references(const ReferenceParams &params);

  private:
    std::unique_ptr<Parser_Impl> pimpl;
};

} // namespace analyzer
} // namespace code
