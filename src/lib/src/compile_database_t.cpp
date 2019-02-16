#include "compile_database_t.hpp"
#include "config.hpp"
#include "filesystem.hpp"
#include "utils.hpp"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <regex>

#include <json.hpp>

namespace code::analyzer {
namespace {
using json = nlohmann::json;
json database()
{

    json parsed_db;
    // compile_commands.json file
    const std::string source =
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

// std::string read_compile_commands(const std::string_view &filename)
// {
//     auto parsed_db = database();
//     if (parsed_db.empty())
//     {
//         return std::string();
//     }
//     // search compile commands associated to the filename
//     for (const auto &it : parsed_db)
//     {
//         if (it["file"] == filename.data())
//         {
//             return it["command"];
//         }
//     }
//     return std::string();
// }

// void split_command(const std::string &command, std::vector<std::string>
// &flags)
// {
//     auto build_dir           = std::filesystem::path(config::build_uri());
//     bool absolutize_argument = false;
//     auto commands            = utils::split(command);
//     // remove the compiler binary name
//     commands.erase(std::begin(commands));
//     for (std::size_t i = 0; i < commands.size(); ++i)
//     {
//         auto __command = commands[i];
//         if (__command.empty())
//         {
//             continue;
//         }
//         if (__command.substr(0, 2) == "-I")
//         {
//             auto path = std::filesystem::path(__command.substr(2));
//             if (path.is_relative())
//             {
//                 // create an absolute path
//                 path = build_dir / path;
//             }
//             // use absolute path
//             __command = "-I" + std::string(path);
//         }
//         if (absolutize_argument)
//         {
//             auto path = std::filesystem::path(__command);
//             if (path.is_relative())
//             {
//                 // create an absolute path
//                 __command = build_dir / path;
//             }
//             absolutize_argument = false;
//         }
//         if (__command.substr(0, 8) == "-isystem")
//         {
//             // used to make absolute next argument
//             absolutize_argument = true;
//         }
//         if (__command == "-o" || __command == "-c")
//         {
//             ++i;
//             continue;
//         }
//         flags.push_back(__command);
//     }
// }

// std::vector<std::string> source_file_commands(const std::string_view
// &filename)
// {
//     std::vector<std::string> flags = config::compile_commands();
//     split_command(read_compile_commands(filename), flags);
//     return flags;
// }

// std::vector<std::string> header_file_commands()
// {
//     std::vector<std::string> flags     = config::compile_commands();
//     auto                     parsed_db = database();
//     for (const auto &it : parsed_db)
//     {
//         split_command(it["command"], flags);
//     }

//     return flags;
// }

// bool is_source(const std::string_view &filename)
// {

//     auto ext = std::filesystem::path(filename).extension();
//     if (ext == ".cpp" || ext == ".cxx" || ext == ".cc" || ext == ".c")
//     {
//         return true;
//     }
//     return false;
// }

// bool is_header(const std::string_view &filename)
// {
//     auto ext = std::filesystem::path(filename).extension();
//     if (ext == ".hpp" || ext == ".hxx" || ext == ".hh" || ext == ".h")
//     {
//         return true;
//     }
//     return false;
// }

// return the last write timestamp
long int last_write_time(const std::string_view &filename)
{
    auto time = std::filesystem::last_write_time(filename);
    return decltype(time)::clock::to_time_t(time);
}

} // namespace

compile_database_t::compile_database_t(
    const std::string_view &directory,
    const std::string_view &prefix_compile_command,
    const command_t &       flags_to_ignore)
    : m_compile_commands_json_db{std::string(
          std::filesystem::path(std::string(directory)) /
          "compile_commands.json")}
    , m_prefix_compile_command{prefix_compile_command}
{
    for (const auto &flag : flags_to_ignore)
    {
        if (!flag.empty())
        {
            // concatenate all flags, with '|' as separation for the regex
            m_flags_to_ignore += " " + flag + " |";
        }
    }
    if (!m_flags_to_ignore.empty())
    {
        // remove the last '|'
        m_flags_to_ignore.pop_back();
    }

    update();
}

std::vector<compile_command>
compile_database_t::compile_commands2(const std::string_view &filename)
{
    update();

    std::vector<compile_command> ret;
    for (const auto &cmd : m_compile_commands)
    {
        if (cmd.m_file == filename)
        {
            ret.push_back(cmd);
        }
    }

    return ret;
}

// parse the compile commands
void compile_database_t::parse_compile_commands() noexcept
{
    auto parsed_db = database();
    if (parsed_db.empty())
    {
        return;
    }
    for (const auto &it : parsed_db)
    {
        try
        {
            // read the command from the json database
            auto       cmd = it.at("command").get<std::string>();
            std::regex flags{m_flags_to_ignore};
            // replace flags to ignore by a space
            cmd = std::regex_replace(cmd, flags, " ");
            // remove the binary compiler name
            auto index = cmd.find(" ");
            cmd        = cmd.substr(index);
            // add prefix_compile_command
            cmd = m_prefix_compile_command + " " + cmd;
            auto _command =
                compile_command(it.at("directory").get<std::string>(),
                                cmd,
                                it.at("file").get<std::string>());
            // add the compile commands for the file
            m_compile_commands.emplace(_command);
        }
        catch (...)
        {
        }
    }
}

void compile_database_t::extract_includes()
{
    // use set to have unique string
    std::set<std::string> __all_includes;
    for (const auto &cc : m_compile_commands)
    {
        bool next{false};
        for (const auto &command : cc.m_command)
        {
            if (command.substr(0, 2) == "-I")
            {
                __all_includes.emplace(command);
            }
            if (next)
            {
                __all_includes.emplace(command);
                next = false;
            }
            if (command == "-isystem")
            {
                next = true;
            }
        }
    }
    for (auto &include : __all_includes)
    {
        if (include.substr(0, 2) == "-I")
        {
            m_all_includes.push_back(std::move(include));
        }
        else
        {
            // if there is no -I means it a system header
            m_all_includes.push_back("-isystem");
            m_all_includes.push_back(std::move(include));
        }
    }
}

void compile_database_t::update()
{
    if (auto current_timestamp = last_write_time(m_compile_commands_json_db);
        m_timestamp != current_timestamp)
    {
        // (re)parse the compile commands file
        parse_compile_commands();
        // once compile command is parsed, extract includes;
        extract_includes();
        m_timestamp = current_timestamp;
    }
}

// std::vector<std::string>
// compile_database_t::compile_commands(const std::string_view &filename)
// {
//     std::vector<std::string> flags;

//     if (is_source(filename))
//     {
//         flags = source_file_commands(filename);
//     }
//     else if (is_header(filename))
//     {
//         flags = header_file_commands();
//     }

//     // remove flags that can lead to an ASTRead Error
//     for (const auto &value : config::flags_to_ignore())
//     {
//         std::remove(std::begin(flags), std::end(flags), value);
//     }
//     return flags;
// }

// std::vector<std::string> compile_database_t::source_filenames()
// {
//     std::vector<std::string> filenames;
//     auto                     parsed_db = database();
//     if (parsed_db.empty())
//     {
//         // empty vector
//         return filenames;
//     }
//     for (const auto &it : parsed_db)
//     {
//         filenames.push_back(it["file"]);
//     }
//     return filenames;
// }
} // namespace code::analyzer
