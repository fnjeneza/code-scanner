#pragma once

#include <string>

namespace code {
namespace analyzer {

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

} // namespace analyzer
} // namespace code
