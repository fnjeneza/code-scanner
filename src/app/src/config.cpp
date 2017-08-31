#include "config.hpp"

#include <fstream>
#include <iostream>

namespace config
{
std::vector<std::string> compile_commands(const std::string & config_path)
{
    const std::string default_config_file{"code-scanner.conf"};
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
            std::string line;
            while(std::getline(in, line, ' '))
            {
                if(!line.empty() && line != " ")
                {
                    flags.push_back(line);
                }
            }
        }
    }

    return flags;
}
}
