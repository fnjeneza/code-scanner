#pragma once

#include <string>
#include <vector>

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

struct CodeLensOptions
{
    bool resolveProvider;
};

struct DocumentOnTypeFormattingOptions
{
    std::string              firstTriggerCharacter;
    std::vector<std::string> moreTriggerCharacter;
};

struct DocumentLinkOptions
{
    bool resolveProvider;
};

struct ExecuteCommandOptions
{
    std::vector<std::string> commands;
};

struct SignatureHelpOptions
{
    std::vector<std::string> triggerCharacters;
};

struct CompletionOptions
{
    bool                     resolverProvider;
    std::vector<std::string> triggerCharacters;
};

struct SaveOptions
{
    bool includeText;
};

struct TextDocumentSyncOptions
{
    bool        openClose;
    std::size_t change;
    bool        willSave;
    bool        willSaveWaitUntil;
    SaveOptions save;
};

struct ServerCapabilities
{
    TextDocumentSyncOptions         textDocumentSync;
    bool                            hoverProvider;
    CompletionOptions               completionProvide;
    SignatureHelpOptions            signatureHelpProvider;
    bool                            definitionProvider;
    bool                            referencesProvider;
    bool                            documentHighlightProvider;
    bool                            documentSymbolProvider;
    bool                            workspaceSymbolProvider;
    bool                            codeActionProvider;
    CodeLensOptions                 codeLensProvider;
    bool                            documentFormattingProvider;
    bool                            documentRangeFormattingProvider;
    DocumentOnTypeFormattingOptions documentOnTypeFormattingOptions;
    bool                            renameProvider;
    DocumentLinkOptions             documentLinkProvider;
    ExecuteCommandOptions           executeCommandProvider;
    std::string                     experimental;
};

struct InitializeResult
{
    ServerCapabilities capabilities;
};

struct Capabilities
{
    WorkspaceClientCapabilities    workspace;
    TextDocumentClientCapabilities textDocument;
    std::string                    experimental;
};

} // namespace analyzer
} // namespace code
