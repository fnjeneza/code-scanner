#include "code-scanner/code-scanner.hpp"

#include <iostream>
#include <string>
#include <vector>

#include <json.hpp>

#include "Parser_Impl.hpp"
#include "code-scanner/Params.hpp"
#include "utils.hpp"

namespace code {
namespace analyzer {

Parser::Parser()
    : pimpl{std::make_unique<Parser_Impl>()}
{
}

Parser::~Parser() = default;

std::string Parser::initialize(const InitializeParams &params)
{
    // compile arguments from initializationOptions
    std::vector<std::string> compile_arguments;

    if (params.initializationOptions.empty())
    {
        return "Missing initialization options";
    }

    using json = nlohmann::json;
    json          conf;
    std::ifstream in(params.initializationOptions);
    if (!in)
    {
        return "can not open " + params.initializationOptions;
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

        auto flags_to_ignore = conf.at("ignore_flags");
        pimpl->initialize(build_uri, compile_arguments, flags_to_ignore);
    }
    catch (const std::exception &e)
    {
        return e.what();
    }
    return std::string();
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
