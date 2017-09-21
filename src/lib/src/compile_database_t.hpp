#include <string>
#include <vector>

namespace code {
namespace analyzer {
struct compile_database_t
{
    static std::vector<std::string>
    compile_commands(const std::string &filename);

    static std::vector<std::string> source_filenames();
};
}
}
