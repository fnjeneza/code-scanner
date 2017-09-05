#pragma once

#include <string>
#include <vector>
#include <memory>

namespace code {
namespace analyzer {

using DocumentUri = std::string;

struct TextDocumentIdentifier{
    DocumentUri uri;
};

struct Position{
    unsigned long line;
    unsigned long character;
};

struct Range{
    Position start;
    Position end;
};

struct Location{
    DocumentUri uri;
    Range range;
};

struct TextDocumentPositionParams{
    TextDocumentIdentifier textDocument;
    Position position;

    // TODO temporary
    std::string build_dir;
    std::vector<std::string> compile_arguments;
};

struct ReferenceContext {
  bool includeDeclaration;
};

struct ReferenceParams: public TextDocumentPositionParams {
  ReferenceContext context;
};

struct Parser_Impl;

class Parser{
    public:
        Parser();
        ~Parser();
        Location definition(const TextDocumentPositionParams & params );
        Location references(const ReferenceParams & params );
    private:
        std::unique_ptr<Parser_Impl> pimpl;
};

} // namespace analyzer
} // namespace code
