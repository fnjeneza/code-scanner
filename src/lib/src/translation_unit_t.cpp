#include "translation_unit_t.hpp"
#include <iostream>
#include <tuple>
#include "functional.hpp"

namespace code{
namespace analyzer{

translation_unit_t::translation_unit_t(const std::string & filename, const std::vector<std::string> & compile_commands)
    : m_unit{nullptr}
    , m_filename{filename}
    , m_compile_commands{compile_commands}
{
}

translation_unit_t::~translation_unit_t()
{
    clang_disposeTranslationUnit(m_unit);
}

void translation_unit_t::parse()
{
    // convert to "const char *" understable by parseTranslationUnit
    std::vector<const char *> flags;
    for (const auto &flag : m_compile_commands)
    {
        flags.push_back(flag.c_str());
    }

    // create index
    auto index = clang_createIndex(1, 1);

    auto error = clang_parseTranslationUnit2FullArgv(
        index,
        m_filename.c_str(),
        &flags[0],
        static_cast<int>(flags.size()),
        nullptr,
        0,
        CXTranslationUnit_SkipFunctionBodies,
        &m_unit);

    switch (error)
    {
    case CXError_Success:
        std::cout << "Succeeded\n";
        break;
    case CXError_InvalidArguments:
        std::cout << "Invalid argument\n";
        break;
    case CXError_ASTReadError:
        std::cout << "ASTRead Error\n";
        break;
    default:
        std::cout << "Error default\n";
        break;
    }

    // release index
    clang_disposeIndex(index);
}

std::set<std::string> translation_unit_t::retrieve_all_identifier_usr()
{
    parse();

    // get translation unit cursor
    CXCursor unit_cursor = clang_getTranslationUnitCursor(m_unit);

    std::set<std::string> identifiers;
    using data = std::tuple<const std::string&, std::set<std::string>&>;
    data _data {m_filename, identifiers};

    clang_visitChildren(
        unit_cursor,
        // visitor
        [](CXCursor cursor_, CXCursor /*parent*/, CXClientData client_data) {

            data *__data = static_cast<data*>(client_data);
            // retrieve the filename
            auto __filename  = std::get<0>(*__data);
            std::string  str      = to_string(clang_getCursorUSR(cursor_));
            if (!str.empty() &&
                is_identifier(cursor_))
            {
                auto loc = location(cursor_);
                if (__filename == std::get<0>(loc)
                    && is_declaration_locate_in_other_file(cursor_))
                {
                      std::cout << to_string(clang_getCursorSpelling(cursor_)) << " ";
                      std::cout << std::get<0>(loc) << ":" << std::get<1>(loc)
                                << ":" << std::get<2>(loc) << std::endl;
                }
            }

            return CXChildVisit_Recurse;
        },
        &_data);
    return identifiers;
}

}}
