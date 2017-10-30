#ifndef UTILS_HPP
#define UTILS_HPP

#include "code-scanner/Location.hpp"
#include <clang-c/Index.h>
#include <string>
#include <vector>

namespace code {
namespace analyzer {
namespace utils {
std::vector<std::string> split(const std::string &argument);
std::string              to_string(const CXString &cx_str);
Location                 location(const CXCursor &cursor);
bool                     is_identifier(const CXCursor &cursor);
bool is_declaration_locate_in_other_file(const CXCursor &cursor);
} // namespace utils
} // namespace analyzer
} // namespace code

#endif /* UTILS_HPP */
