#include "config.hpp"
#include <iostream>

using flags = std::vector<std::string>;

namespace {
static std::string __build_uri;
static flags       __compile_commands;
static flags       __flags_to_ignore;
}

void config::builder(const std::string &build_uri,
                     const flags &      compile_commands,
                     const flags &      flags_to_ignore)
{
    __build_uri        = build_uri;
    __compile_commands = compile_commands;
    __flags_to_ignore  = flags_to_ignore;
}

flags config::flags_to_ignore() { return __flags_to_ignore; }

flags config::compile_commands() { return __compile_commands; }

std::string config::build_uri() { return __build_uri; }
