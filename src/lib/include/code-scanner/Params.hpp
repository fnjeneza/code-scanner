#pragma once

#include "Capabilities.hpp"
#include "Position.hpp"

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
    bool includeDeclaration{true};
};

struct ReferenceParams : public TextDocumentPositionParams
{
    ReferenceParams()  = default;
    ~ReferenceParams() = default;
    ReferenceParams(const TextDocumentPositionParams &params)
    {
        textDocument = params.textDocument;
        position     = params.position;
    }
    ReferenceParams(TextDocumentPositionParams &&params)
    {
        textDocument = std::move(params.textDocument);
        position     = std::move(params.position);
    }

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
