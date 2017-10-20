#include "code-scanner/code-scanner.hpp"

#include <experimental/filesystem>
#include <iostream>
#include <string>
#include <vector>

#include <json.hpp>

#include "Parser_Impl.hpp"
#include "code-scanner/ErrorCodes.hpp"
#include "code-scanner/Params.hpp"
#include "utils.hpp"

namespace std {
namespace filesystem = std::experimental::filesystem;
}

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
        return error("Missing initialization options");
    }

    using json = nlohmann::json;
    json          conf;
    std::ifstream in(params.initializationOptions);
    if (!in)
    {
        return error("can not open " + params.initializationOptions);
    }
    in >> conf;
    try
    {
        auto build_uri = conf.at("build_uri").get<std::string>();

        {
            // check that compile_commands.json exists
            auto __path =
                std::filesystem::path(build_uri) / "compile_commands.json";
            if (!std::filesystem::exists(__path))
            {
                return error(std::string(__path) + " file not exists");
            }
        }

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
        return error(ErrorCodes::ParserError, e.what());
    }
    // set initialized
    m_initialized = true;
    return std::nullopt;
}

Location Parser::definition(const TextDocumentPositionParams &params)
{
    if (!m_initialized)
    {
        Location();
    }
    return pimpl->definition(params);
}

Location Parser::references(const ReferenceParams &params)
{
    if (!m_initialized)
    {
        Location();
    }
    return pimpl->reference(params);
}

} // namespace analyzer
} // namespace code
