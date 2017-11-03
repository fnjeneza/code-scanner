#include "compile_command.hpp"
#include <string>
#include <vector>
#include <set>

namespace code::analyzer {

using command_t = std::vector<std::string>;
struct compile_database_t
{
    compile_database_t() = delete;
    compile_database_t(const std::string_view &directory);
    ~compile_database_t() = default;

    static std::vector<std::string>
    compile_commands(const std::string_view &filename);

    static std::vector<std::string> source_filenames();

    // retrieve the compile commands for a given filename
    std::vector<command_t> compile_commands2(const std::string_view &filename);
    void                   parse_compile_commands() noexcept;

    // build dir filename
    std::string                  m_compile_commands_json_db;
    long int                     m_timestamp = 0;
    std::set<compile_command> m_compile_commands;
};
} // namespace code::analyzer
