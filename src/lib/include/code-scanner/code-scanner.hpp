#pragma once

#include <memory>
#include <string>
#include <vector>

namespace code {
namespace analyzer {

using DocumentUri = std::string;

struct TextDocumentIdentifier
{
    DocumentUri uri;
};

struct Position
{
    unsigned int line;
    unsigned int character;
};

struct Range
{
    Position start;
    Position end;
};

struct Location
{
    DocumentUri uri;
    Range       range;
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

struct DynamicRegistration
{
    bool dynamicRegistration;
};

struct WorkspaceEdit
{
    bool documentChanges;
};

struct WorkspaceClientCapabilities
{
    bool                applyEdit;
    WorkspaceEdit       workspaceEdit;
    DynamicRegistration didChangeConfiguration;
    DynamicRegistration didChangeWatchedFiles;
    DynamicRegistration symbol;
    DynamicRegistration executeCommand;
};

struct CompletionItem
{
    bool snippetSupport;
};

struct Completion : public DynamicRegistration
{
    CompletionItem completionItem;
};

struct Synchronization : public DynamicRegistration
{
    bool willSave;
    bool WillSaveWaituntil;
    bool didSave;
};

struct TextDocumentClientCapabilities
{
    Synchronization     synchronization;
    Completion          completion;
    DynamicRegistration hover;
    DynamicRegistration signatureHelp;
    DynamicRegistration references;
    DynamicRegistration documentHighlight;
    DynamicRegistration documentSymbol;
    DynamicRegistration formatting;
    DynamicRegistration rangeFormatting;
    DynamicRegistration ontypeFormatting;
    DynamicRegistration definition;
    DynamicRegistration codeAction;
    DynamicRegistration codeLens;
    DynamicRegistration documentLink;
    DynamicRegistration rename;
};

struct Capabilities
{
    WorkspaceClientCapabilities    workspace;
    TextDocumentClientCapabilities textDocument;
    std::string                    experimental;
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

struct Parser_Impl;

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
