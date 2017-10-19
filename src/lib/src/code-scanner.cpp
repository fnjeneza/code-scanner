#include "code-scanner/code-scanner.hpp"

#include <iostream>
#include <string>
#include <vector>

#include <json.hpp>

#include "Parser_Impl.hpp"
#include "code-scanner/ErrorCodes.hpp"
#include "code-scanner/Params.hpp"
#include "utils.hpp"

namespace code {
namespace analyzer {

Parser::Parser()
    : pimpl{std::make_unique<Parser_Impl>()}
{
}

Parser::~Parser() = default;

std::optional<std::error_code>
Parser::initialize(const InitializeParams &params)
{
    // compile arguments from initializationOptions
    std::vector<std::string> compile_arguments;

    if (params.initializationOptions.empty())
    {
        // "Missing initialization options";
        return ErrorCodes::InvalidParams;
    }

    using json = nlohmann::json;
    json          conf;
    std::ifstream in(params.initializationOptions);
    if (!in)
    {
        // "can not open " + params.initializationOptions;
        return ErrorCodes::InvalidParams;
    }
    in >> conf;
    try
    {
        auto build_uri        = conf.at("build_uri");
        auto compile_commands = conf.at("compile_commands");

        if (!compile_commands.empty())
        {
            compile_arguments = utils::split(compile_commands);
        }

        using flags          = std::vector<std::string>;
        auto flags_to_ignore = conf.at("ignore_flags").get<flags>();
        pimpl->initialize(build_uri, compile_arguments, flags_to_ignore);
    }
    catch (const std::exception &e)
    {
        // e.what();
        return ErrorCodes::ParserError;
    }
    return std::nullopt;
}

Location Parser::definition(const TextDocumentPositionParams &params)
{
    return pimpl->definition(params);
}

Location Parser::references(const ReferenceParams &params)
{
    return pimpl->reference(params);
}

} // namespace analyzer
} // namespace code
