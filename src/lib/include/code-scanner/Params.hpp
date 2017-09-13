#pragma once

#include "Capabilities.hpp"

#include <string>

namespace code {
namespace analyzer {

using DocumentUri = std::string;

struct TextDocumentIdentifier
{
    DocumentUri uri;
};

struct TextDocumentPositionParams
{
    TextDocumentIdentifier textDocument;
    Position               position;
};

struct ReferenceContext
{
    bool includeDeclaration;
};

struct ReferenceParams : public TextDocumentPositionParams
{
    ReferenceContext context;
};

enum class Trace
{
    off,
    messages,
    verbose
};

struct InitializeParams
{
    std::size_t  processId;
    DocumentUri  rootUri;
    std::string  initializationOptions; // json config options
    Capabilities capabilities;
    Trace        trace;
};

} // namespace analyzer
} // namespace code
