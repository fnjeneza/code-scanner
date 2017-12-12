#pragma once

#include <string>
#include <vector>

namespace code {
namespace analyzer {

struct DynamicRegistration
{
    bool dynamicRegistration = false;
};

struct WorkspaceEdit
{
    bool documentChanges = false;
};

struct WorkspaceClientCapabilities
{
    bool                applyEdit = false;
    WorkspaceEdit       workspaceEdit;
    DynamicRegistration didChangeConfiguration;
    DynamicRegistration didChangeWatchedFiles;
    DynamicRegistration symbol;
    DynamicRegistration executeCommand;
};

struct CompletionItem
{
    bool snippetSupport = false;
};

struct Completion : public DynamicRegistration
{
    CompletionItem completionItem;
};

struct Synchronization : public DynamicRegistration
{
    bool willSave          = false;
    bool WillSaveWaituntil = false;
    bool didSave           = false;
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
    bool resolveProvider = false;
};

struct DocumentOnTypeFormattingOptions
{
    std::string              firstTriggerCharacter;
    std::vector<std::string> moreTriggerCharacter;
};

struct DocumentLinkOptions
{
    bool resolveProvider = false;
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
    bool                     resolveProvider = false;
    std::vector<std::string> triggerCharacters;
};

struct SaveOptions
{
    bool includeText = false;
};

struct TextDocumentSyncOptions
{
    bool        openClose = false;
    std::size_t change            = 0;
    bool        willSave          = false;
    bool        willSaveWaitUntil = false;
    SaveOptions save;
};

struct ServerCapabilities
{
    TextDocumentSyncOptions         textDocumentSync;
    bool                            hoverProvider = false;
    CompletionOptions               completionProvider;
    SignatureHelpOptions            signatureHelpProvider;
    bool                            definitionProvider        = false;
    bool                            referencesProvider        = false;
    bool                            documentHighlightProvider = false;
    bool                            documentSymbolProvider    = false;
    bool                            workspaceSymbolProvider   = false;
    bool                            codeActionProvider        = false;
    CodeLensOptions                 codeLensProvider;
    bool                            documentFormattingProvider      = false;
    bool                            documentRangeFormattingProvider = false;
    DocumentOnTypeFormattingOptions documentOnTypeFormattingProvider;
    bool                            renameProvider = false;
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
