#include "config.hpp"

#include <fstream>
#include <iostream>
#include <json.hpp>

#include "utils.hpp"

namespace config
{
std::vector<std::string> compile_commands(const std::string & config_path)
{
    const std::string default_config_file{"config.json"};
    std::vector<std::string> flags;
    std::ifstream in;

    in.open(config_path);
    if (!in.is_open())
    {
        // open default config file
        in.open(default_config_file);
        if(!in.is_open())
        {
            // no file has been opened
            std::cout << "can not open config file\n";
        }
        else
        {
            using json = nlohmann::json;
            // json object = {{"compile_commands", "clang -x c++ -std=c++14"}};
            json conf;
            in >> conf;
            auto compile_commands = conf["compile_commands"];
            if(!compile_commands.empty())
            {
                flags = utils::split(compile_commands);
            }
        }
    }

    return flags;
}
}
