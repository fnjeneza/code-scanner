#include "code-scanner/code-scanner.hpp"

#include <algorithm>
#include <iostream>
#include <string>
#include <tuple>
#include <unordered_set>
#include <vector>

#include <json.hpp>

#include <clang-c/CXCompilationDatabase.h>
#include <clang-c/Index.h>

#include "code-scanner/Params.hpp"
#include "utils.hpp"
#include "Parser_Impl.hpp"
#include "functional.hpp"

namespace {
std::string to_string(const CXString &cx_str)
{
    auto cstr = clang_getCString(cx_str);

    if (cstr == NULL)
    {
        return std::string();
    }

    std::string str(cstr);
    clang_disposeString(cx_str);
    return str;
}
} // anonymous namespace

namespace code {
namespace analyzer {

Parser::Parser()
    : pimpl{std::make_unique<Parser_Impl>()}
{
}

Parser::~Parser() = default;

void Parser::initialize(const InitializeParams &params)
{
    const std::string root_uri = params.rootUri;
    // compile arguments from initializationOptions
    std::vector<std::string> compile_arguments;

    using json = nlohmann::json;
    if (params.initializationOptions.empty())
    {
        std::cerr << "Missing initialization options" << std::endl;
        // TODO return error code
        return;
    }

    json conf             = json::parse(params.initializationOptions);
    auto compile_commands = conf["compile_commands"];

    if (!compile_commands.empty())
    {
        compile_arguments = utils::split(compile_commands);
    }

    auto flags_to_ignore = conf["ignore_flags"];
    pimpl->initialize(root_uri, compile_arguments, flags_to_ignore);
}

Location Parser::definition(const TextDocumentPositionParams &params)
{
    pimpl->parse(params.textDocument.uri);
    auto cursor = pimpl->cursor(params.textDocument.uri,
                                params.position.line,
                                params.position.character);
    CXCursor found = code::analyzer::definition(cursor);

    // retrieve location from CXCursor
    auto get_location = [](CXCursor &_cursor) {

        auto loc = location(_cursor);

        Position position;
        position.line      = std::get<1>(loc);
        position.character = std::get<2>(loc);

        Range range;
        range.start = position;
        range.end   = position;

        Location _location;
        _location.uri   = std::get<0>(loc);
        _location.range = range;
        return _location;
    };

    // if a cursor has been found
    if (!clang_Cursor_isNull(found))
    {
        return get_location(found);
    }

    auto              cu        = clang_getCursorReferenced(cursor);
    auto loc = location(cu);
    auto decl_filename = std::get<0>(loc);
    auto decl_line = std::get<1>(loc);
    auto decl_column = std::get<2>(loc);


    const std::string usr       = to_string(clang_getCursorUSR(cu));
    auto              filenames = pimpl->get_all_filenames();

    std::size_t i = 1;
    std::size_t total = filenames.size();
    for (auto f : filenames)
    {
        std::cout << i << "/" << total << " ";
        clang_disposeTranslationUnit(pimpl->m_unit);
        pimpl->parse(f);
        CXCursor present = pimpl->cursor(decl_filename, decl_line, decl_column);
        if(!clang_Cursor_isNull(present))
        {
          found = clang_getCursorDefinition(present);
        }

        if (clang_Cursor_isNull(found))
        {
            ++i;
            continue;
        }
        return get_location(found);
    }

    Location location;
    return location;
}

Location Parser::references(const ReferenceParams &params)
{
    pimpl->parse(params.textDocument.uri);
    auto cursor = pimpl->cursor(params.textDocument.uri,
                                params.position.line,
                                params.position.character);
    cursor   = code::analyzer::reference(cursor);
    auto loc = location(cursor);

    Position position;
    position.line      = std::get<1>(loc);
    position.character = std::get<2>(loc);

    Range range;
    range.start = position;
    range.end   = position;

    Location _location;
    _location.uri   = std::get<0>(loc);
    _location.range = range;
    return _location;
}

} // namespace analyzer
} // namespace code
