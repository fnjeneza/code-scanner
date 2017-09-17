#include <vector>
#include <set>
#include <string>
#include <clang-c/Index.h>

namespace code{
namespace analyzer{
class translation_unit_t
{
public:
    translation_unit_t(const std::string & filename, const std::vector<std::string> & compile_commands);
    ~translation_unit_t();

    std::set<std::string> retrieve_all_identifier_usr();

private:
    void parse();

private:
    CXTranslationUnit m_unit;
    std::string m_filename;
    std::vector<std::string> m_compile_commands;

};
}}
