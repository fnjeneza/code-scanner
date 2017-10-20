#include "config.hpp"
#include "code-scanner/ErrorCodes.hpp"
#include <experimental/filesystem>

namespace std {
namespace filesystem = std::experimental::filesystem;
}

using flags = std::vector<std::string>;

namespace {
static std::string __build_uri;
static flags       __compile_commands;
static flags       __flags_to_ignore;
}

std::optional<std::error_code> config::builder(const std::string &build_uri,
                                               const flags &compile_commands,
                                               const flags &flags_to_ignore)
{
    __build_uri = build_uri;
    {
        // check that compile_commands.json exists
        auto __path =
            std::filesystem::path(__build_uri) / "compile_commands.json";
        if (!std::filesystem::exists(__path))
        {
            return error(std::string(__path) + " file not exists");
        }
    }
    __compile_commands = compile_commands;
    __flags_to_ignore  = flags_to_ignore;
    return std::nullopt;
}

flags config::flags_to_ignore() { return __flags_to_ignore; }

flags config::compile_commands() { return __compile_commands; }

std::string config::build_uri() { return __build_uri; }
