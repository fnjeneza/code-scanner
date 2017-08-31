#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <vector>
#include <string>

// why using template?
// use templqte here
// and forward declaration
namespace config
{
std::vector<std::string> compile_commands(const std::string & config_path="");
}

#endif /* CONFIG_HPP */
