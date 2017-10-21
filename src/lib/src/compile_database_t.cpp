#include "compile_database_t.hpp"
#include "config.hpp"
#include "utils.hpp"

#include <algorithm>
#include <experimental/filesystem>
#include <fstream>
#include <iostream>

#include <json.hpp>

namespace std {
namespace filesystem = std::experimental::filesystem;
}

namespace code::analyzer {
namespace {
using json = nlohmann::json;
json database()
{

    json parsed_db;
    // compile_commands.json file
    std::string source =
        std::filesystem::path(config::build_uri()) / "compile_commands.json";
    std::ifstream db_file(source.c_str());
    if (!db_file)
    {
        std::cout << "can not read " << source << std::endl;
        return json();
    }
    db_file >> parsed_db;
    return parsed_db;
}

std::string read_compile_commands(const std::string &filename)
{
    auto parsed_db = database();
    if (parsed_db.empty())
    {
        return std::string();
    }
    // search compile commands associated to the filename
    for (const auto &it : parsed_db)
    {
        if (it["file"] == filename)
        {
            return it["command"];
        }
    }
    return std::string();
}

void split_command(const std::string &command, std::vector<std::string> &flags)
{
    auto build_dir           = std::filesystem::path(config::build_uri());
    bool absolutize_argument = false;
    auto commands            = utils::split(command);
    // remove the compiler binary name
    commands.erase(std::begin(commands));
    for (std::size_t i = 0; i < commands.size(); ++i)
    {
        auto __command = commands[i];
        if (__command.empty())
        {
            continue;
        }
        if (__command.substr(0, 2) == "-I")
        {
            auto path = std::filesystem::path(__command.substr(2));
            if (path.is_relative())
            {
                // create an absolute path
                path = build_dir / path;
            }
            // use absolute path
            __command = "-I" + std::string(path);
        }
        if (absolutize_argument)
        {
            auto path = std::filesystem::path(__command);
            if (path.is_relative())
            {
                // create an absolute path
                __command = build_dir / path;
            }
            absolutize_argument = false;
        }
        if (__command.substr(0, 8) == "-isystem")
        {
            // used to make absolute next argument
            absolutize_argument = true;
        }
        if (__command == "-o" || __command == "-c")
        {
            ++i;
            continue;
        }
        flags.push_back(__command);
    }
}

std::vector<std::string> source_file_commands(const std::string &filename)
{
    std::vector<std::string> flags = config::compile_commands();
    split_command(read_compile_commands(filename), flags);
    return flags;
}

std::vector<std::string> header_file_commands()
{
    std::vector<std::string> flags     = config::compile_commands();
    auto                     parsed_db = database();
    for (const auto &it : parsed_db)
    {
        split_command(it["command"], flags);
    }

    return flags;
}

bool is_source(const std::string &filename)
{
    auto index = filename.rfind('.');
    if (index == std::string::npos)
    {
        return false;
    }
    auto ext = filename.substr(index);
    if (ext == ".cpp" || ext == ".cxx" || ext == ".cc" || ext == ".c")
    {
        return true;
    }
    return false;
}
} // namespace

std::vector<std::string>
compile_database_t::compile_commands(const std::string &filename)
{
    std::vector<std::string> flags;

    if (is_source(filename))
    {
        flags = source_file_commands(filename);
    }
    else
    {
        flags = header_file_commands();
    }

    // remove flags that can lead to an ASTRead Error
    for (const auto &value : config::flags_to_ignore())
    {
        std::remove(std::begin(flags), std::end(flags), value);
    }
    return flags;
}

std::vector<std::string> compile_database_t::source_filenames()
{
    std::vector<std::string> filenames;
    auto                     parsed_db = database();
    if (parsed_db.empty())
    {
        // empty vector
        return filenames;
    }
    for (const auto &it : parsed_db)
    {
        filenames.push_back(it["file"]);
    }
    return filenames;
}
} // namespace code::analyzer
