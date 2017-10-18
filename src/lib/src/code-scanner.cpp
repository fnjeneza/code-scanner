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

void Parser::initialize(const InitializeParams &params)
{
    // compile arguments from initializationOptions
    std::vector<std::string> compile_arguments;

    if (params.initializationOptions.empty())
    {
        std::cerr << "Missing initialization options" << std::endl;
        // TODO return error code
        return;
    }

    using json = nlohmann::json;
    json          conf;
    std::ifstream in(params.initializationOptions);
    if (!in)
    {
        std::cerr << "can not open " << params.initializationOptions
                  << std::endl;
        return;
    }
    in >> conf;
    try
    {
        auto build_dir        = conf.at("build_dir");
        auto compile_commands = conf.at("compile_commands");

        if (!compile_commands.empty())
        {
            compile_arguments = utils::split(compile_commands);
        }

        auto flags_to_ignore = conf.at("ignore_flags");
        pimpl->initialize(build_dir, compile_arguments, flags_to_ignore);
    }
    catch (const std::exception &e)
    {
        std::cout << e.what() << std::endl;
        // TODO return error here
        return;
    }
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
