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

    json conf;
    std::ifstream in(params.initializationOptions);
    if(!in)
    {
      std::cerr << "can not open " << params.initializationOptions << std::endl;
      return;
    }
    in >> conf;
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
    return pimpl->definition(params);
}

Location Parser::references(const ReferenceParams &params)
{
    return pimpl->reference(params);
}

} // namespace analyzer
} // namespace code
