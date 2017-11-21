#include "translation_unit_t.hpp"
#include "code-scanner/Position.hpp"
#include "compile_database_t.hpp"
#include "utils.hpp"
#include <iostream>
#include <tuple>

#include <clang-c/Index.h>

namespace code {
namespace analyzer {

namespace {
CXTranslationUnit_Flags option(const translation_unit_flag &flag)
{
    switch (flag)
    {
    case translation_unit_flag::skip_function_bodies:
        return CXTranslationUnit_SkipFunctionBodies;
        // CXTranslationUnit_DetailedPreprocessingRecord;
    case translation_unit_flag::none:
    default:
        return CXTranslationUnit_None;
    }
}

CXCursor cursor(const CXTranslationUnit &unit,
                const std::string_view & filename,
                const Position &         position)
{
    CXFile file = clang_getFile(unit, filename.data());
    if (file == nullptr)
    {
        return clang_getNullCursor();
    }
    CXSourceLocation __location =
        clang_getLocation(unit, file, position.line, position.character);
    return clang_getCursor(unit, __location);
}
} // namespace

translation_unit_t::translation_unit_t(const compile_command &cmd,
                                       const bool skip_function_bodies)
    : m_compile_cmd{cmd}
{
    if (!skip_function_bodies)
    {
        parse();
    }
    else
    {
        parse(translation_unit_flag::skip_function_bodies);
    }
}

translation_unit_t::~translation_unit_t()
{
    clang_disposeTranslationUnit(m_unit);
}

void translation_unit_t::compile_cmd(const compile_command &cmd)
{
    if (m_compile_cmd != cmd)
    {
        m_compile_cmd = cmd;
        parse();
    }
}

void translation_unit_t::parse(const translation_unit_flag &opt)
{
    auto &__flags = m_compile_cmd.m_command;

    // convert to "const char *" understable by parseTranslationUnit
    std::vector<const char *> flags;
    for (const auto &flag : __flags)
    {
        flags.push_back(flag.c_str());
    }

    // create index
    auto index = clang_createIndex(1, 1);

    auto error = clang_parseTranslationUnit2FullArgv(
        index,
        m_compile_cmd.m_file.c_str(),
        flags.data(),
        static_cast<int>(flags.size()),
        nullptr,
        0,
        CXTranslationUnit_DetailedPreprocessingRecord,
        // option(opt),
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

Location translation_unit_t::definition(const Position &position) const
{
    auto _cursor = cursor(m_unit, m_compile_cmd.m_file, position);
    // search for definition
    return utils::location(clang_getCursorDefinition(_cursor));
}

Location translation_unit_t::definition(const std::string &usr) const
{
    // get translation unit cursor
    CXCursor unit_cursor = clang_getTranslationUnitCursor(m_unit);
    using Data           = std::tuple<const std::string &, Location &>;
    Location location;
    Data     data{usr, location};

    // search definition
    clang_visitChildren(
        unit_cursor,
        // visitor
        [](CXCursor cursor_, CXCursor /*parent*/, CXClientData client_data) {

            auto        __data = static_cast<Data *>(client_data);
            auto &      __usr  = std::get<0>(*__data);
            std::string current_cursor_usr =
                utils::to_string(clang_getCursorUSR(cursor_));
            if (current_cursor_usr == __usr)
            {
                auto &__location = std::get<1>(*__data);
                __location =
                    utils::location(clang_getCursorDefinition(cursor_));
                return CXChildVisit_Break;
            }
            return CXChildVisit_Recurse;
        },
        &data);
    return location;
}

Location translation_unit_t::reference(const Position &position) const
{
    auto _cursor = cursor(m_unit, m_compile_cmd.m_file, position);
    return utils::location(clang_getCursorReferenced(_cursor));
}

std::string translation_unit_t::usr(const Position &position) const
{
    auto _cursor = cursor(m_unit, m_compile_cmd.m_file, position);
    return utils::to_string(
        clang_getCursorUSR(clang_getCursorReferenced(_cursor)));
}

std::set<std::string> translation_unit_t::retrieve_all_identifier_usr() const
{
    // get translation unit cursor
    CXCursor unit_cursor = clang_getTranslationUnitCursor(m_unit);

    std::set<std::string> identifiers;
    using data = std::tuple<const std::string &, std::set<std::string> &>;
    data _data{m_compile_cmd.m_file, identifiers};

    clang_visitChildren(
        unit_cursor,
        // visitor
        [](CXCursor cursor_, CXCursor /*parent*/, CXClientData client_data) {

            data *__data = static_cast<data *>(client_data);
            // retrieve the filename
            const auto &__filename    = std::get<0>(*__data);
            auto &      __identifiers = std::get<1>(*__data);
            std::string str = utils::to_string(clang_getCursorUSR(cursor_));
            if (!str.empty() && utils::is_identifier(cursor_))
            {
                auto __location = utils::location(cursor_);
                if (__filename == __location.uri &&
                    utils::is_declaration_locate_in_other_file(cursor_))
                {
                    // append the USR
                    __identifiers.emplace(str);
                }
            }

            return CXChildVisit_Recurse;
        },
        &_data);
    return identifiers;
}
using T = std::set<symbol>;
struct data_t
{
    T               _symbols;
    compile_command _compile_command;
    compile_command compile_command_ref;
};
T translation_unit_t::index_symbols() const
{
    // get translation unit cursor
    CXCursor unit_cursor = clang_getTranslationUnitCursor(m_unit);

    data_t _data;
    _data.compile_command_ref = m_compile_cmd;

    clang_visitChildren(
        unit_cursor,
        // visitor
        [](CXCursor cursor_, CXCursor /*parent*/, CXClientData client_data) {
            // check if the location is in the system header. If so it will
            // be ignored
            CXSourceLocation _location = clang_getCursorLocation(cursor_);
            if (clang_Location_isInSystemHeader(_location))
            {
                return CXChildVisit_Continue;
            }

            data_t *client = static_cast<data_t *>(client_data);
            T *             __data = &client->_symbols;
            auto            kind   = clang_getCursorKind(cursor_);

            // handle InclusionDirective
            if (kind == CXCursor_InclusionDirective)
            {
                // header file path
                auto header_file = utils::to_string(
                    clang_getFileName(clang_getIncludedFile(cursor_)));
                // use ctor in order to have a canonical path
                compile_command header_compile_command(header_file);
                // assign the command
                header_compile_command.m_command =
                    client->compile_command_ref.m_command;
                // assign the directory path
                header_compile_command.m_directory =
                    client->compile_command_ref.m_directory;
                return CXChildVisit_Continue;
            }
            // TODO Build compile command for header

            // auto __location = utils::location(cursor_);

            auto str = utils::to_string(clang_getCursorUSR(cursor_));
            // ignore empty usr
            if (str.empty())
            {
                // return CXChildVisit_Continue;
            }

            // if (!str.empty())
            {
                // check that it is a reference or a definition
                // process only declarations and references
                if (clang_isCursorDefinition(cursor_))
                {
                    auto location = utils::location(cursor_);
                    // std::cout << str << std::endl;
                    // std::cout
                    //     <<
                    //     utils::to_string(clang_getCursorSpelling(cursor_))
                    //     << " " << location.uri << " "
                    //     << location.range.start.line << " "
                    //     << location.range.start.character << " "
                    //     << location.range.end.line << " "
                    //     << location.range.end.character << std::endl;
                    __data->emplace(symbol(str, location, kind::definition));
                }
                // else if (clang_isReference(kind) ||
                // clang_isDeclaration(kind))
                // {
                //     auto location = utils::location(cursor_);
                //     __data->emplace(symbol(str, location,
                //     kind::reference));
                // }
            }

            return CXChildVisit_Recurse;
        },
        &_data);
    return _data._symbols;
}

} // namespace analyzer
} // namespace code
