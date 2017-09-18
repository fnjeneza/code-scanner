#include "Parser_Impl.hpp"

#include <algorithm>
#include <iostream>
#include <tuple>
#include <unordered_set>

#include "config.hpp"
#include "functional.hpp"
#include "translation_unit_t.hpp"

namespace code {
namespace analyzer {

Parser_Impl::Parser_Impl()
    : m_flags{}
    , m_unit{nullptr}
    , m_db{nullptr}
{
}

Parser_Impl::~Parser_Impl()
{
    clang_disposeTranslationUnit(m_unit);
    clang_CompilationDatabase_dispose(m_db);
}

void Parser_Impl::parse(const std::string &filename)
{

    set_flags(filename);
    // convert to "const char *" understable by parseTranslationUnit
    std::vector<const char *> flags;
    for (const auto &flag : m_flags)
    {
        flags.push_back(flag.c_str());
    }
    auto index = clang_createIndex(1, 1);

    auto error = clang_parseTranslationUnit2FullArgv(
        index,
        filename.c_str(),
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
    clang_disposeIndex(index);
}

CXCursor Parser_Impl::locate_definitions(const std::string &filename)
{
    // get translation unit cursor
    CXCursor unit_cursor = clang_getTranslationUnitCursor(m_unit);

    std::string _filename = filename;
    clang_visitChildren(
        unit_cursor,
        // visitor
        [](CXCursor cursor_, CXCursor /*parent*/, CXClientData client_data) {

            std::string *__filename = static_cast<std::string *>(client_data);
            std::string  str        = to_string(clang_getCursorUSR(cursor_));
            if (!str.empty() && is_identifier(cursor_))
            {
                auto loc = location(cursor_);
                if (*__filename == std::get<0>(loc) &&
                    is_declaration_locate_in_other_file(cursor_))
                {
                    std::cout << to_string(clang_getCursorSpelling(cursor_))
                              << " ";
                    std::cout << std::get<0>(loc) << ":" << std::get<1>(loc)
                              << ":" << std::get<2>(loc) << std::endl;
                }
            }

            return CXChildVisit_Recurse;
        },
        &_filename);

    CXCursor cursor;
    return cursor;
}

void Parser_Impl::source_compile_flags(const CXCompileCommand &compile_command)
{
    unsigned number_args = clang_CompileCommand_getNumArgs(compile_command);

    for (unsigned i = 1; i < number_args; ++i)
    {
        std::string str(
            to_string(clang_CompileCommand_getArg(compile_command, i)));
        if (str == "-o" || str == "-c")
        {
            ++i;
            continue;
        }
        m_flags.push_back(str);
    }
}

std::vector<std::string> Parser_Impl::header_compile_flags()
{
    find_all_include_directories();
    std::vector<std::string> flags = m_flags;
    return flags;
}

void Parser_Impl::set_flags(const std::string &filename)
{
    CXCompileCommands compile_commands =
        clang_CompilationDatabase_getCompileCommands(m_db, filename.c_str());

    unsigned size = clang_CompileCommands_getSize(compile_commands);

    if (size != 0) // is source file
    {
        // compile command of the source file
        CXCompileCommand compile_command =
            clang_CompileCommands_getCommand(compile_commands, 0);

        // flags applied to the source file
        source_compile_flags(compile_command);
        clang_CompileCommands_dispose(compile_commands);
    }
    else // is header file
    {
        // TODO better handle errors
        std::cout << "compile command has size 0" << std::endl;
        // flags applied to the header file
        header_compile_flags();
    }

    // remove flags that can lead to an ASTRead Error
    for (const auto &value : config::flags_to_ignore())
    {
        std::remove(std::begin(m_flags), std::end(m_flags), value);
    }
}

void Parser_Impl::initialize(const std::string &             root_uri,
                             const std::vector<std::string> &compile_commands,
                             const std::vector<std::string> &flags_to_ignore)
{
    config::builder(root_uri, compile_commands, flags_to_ignore);
    m_flags = config::compile_commands();

    CXCompilationDatabase_Error c_error = CXCompilationDatabase_NoError;
    m_db = clang_CompilationDatabase_fromDirectory(root_uri.c_str(), &c_error);

    if (c_error == CXCompilationDatabase_CanNotLoadDatabase)
    {
        // TODO Handle errors in ctor
        std::cout << "compilation database can not be loaded" << std::endl;
        return;
    }
}

Location Parser_Impl::definition(const TextDocumentPositionParams &params)
{
    Location location =
        translation_unit_t(params.textDocument.uri).definition(params.position);
    if(location.is_valid())
    {
      std::cout << "no definition found"<< std::endl;
      // search in repository
    }
    return location;
}

Location Parser_Impl::reference(const TextDocumentPositionParams &params)
{
    Location location =
        translation_unit_t(params.textDocument.uri).reference(params.position);
    return location;
}

// Retrieve a cursor from a file/line/column
CXCursor Parser_Impl::cursor(const std::string & filename,
                             const unsigned int &line,
                             const unsigned int &column)
{
    CXFile file = clang_getFile(m_unit, filename.c_str());
    if (file == nullptr)
    {
        return clang_getNullCursor();
    }
    CXSourceLocation location = clang_getLocation(m_unit, file, line, column);
    return clang_getCursor(m_unit, location);
}

std::vector<CXCursor> Parser_Impl::callers(const CXCursor &cursor) const
{
    // get cursor declaration
    CXCursor              cursor_decl = declaration(cursor);
    std::vector<CXCursor> cursors;
    std::tuple<CXCursor *, std::vector<CXCursor> *> cursor_data = {&cursor_decl,
                                                                   &cursors};

    // get translation unit cursor
    CXCursor unit_cursor = clang_getTranslationUnitCursor(m_unit);

    // traverse the AST and check every cursor if it is equal to the
    // cursor declaration
    // ignore the cursor which point to himself
    CXClientData user_data = static_cast<CXClientData>(&cursor_data);
    clang_visitChildren(
        unit_cursor,
        // visitor
        [](CXCursor cursor_, CXCursor, CXClientData client_data) {
            CXCursorKind cursor_kind = clang_getCursorKind(cursor_);
            if (cursor_kind != CXCursor_CallExpr)
            {
                return CXChildVisit_Recurse;
            }
            // current cursor declaration
            CXCursor current_cursor_decl = declaration(cursor_);
            using UserData = std::tuple<CXCursor *, std::vector<CXCursor> *>;
            UserData *             data = static_cast<UserData *>(client_data);
            CXCursor *             cursor_decl_ = std::get<0>(*data);
            std::vector<CXCursor> *cursors_     = std::get<1>(*data);
            unsigned               equal =
                clang_equalCursors(current_cursor_decl, *cursor_decl_);
            if (equal)
            {
                (*cursors_).push_back(cursor_);
            }
            // Continue to search more
            return CXChildVisit_Recurse;
        },
        user_data);

    return cursors;
}

void Parser_Impl::find_all_include_directories()
{
    auto all_compile_commands =
        clang_CompilationDatabase_getAllCompileCommands(m_db);
    auto size = clang_CompileCommands_getSize(all_compile_commands);
    std::unordered_set<std::string> local_include_dirs;
    std::unordered_set<std::string> system_include_dirs;
    for (std::size_t i = 0; i < size; ++i)
    {
        auto command = clang_CompileCommands_getCommand(
            all_compile_commands, static_cast<unsigned int>(i));
        auto number_args = clang_CompileCommand_getNumArgs(command);
        for (std::size_t pos = 1; pos < number_args; ++pos)
        {
            std::string str(to_string(clang_CompileCommand_getArg(
                command, static_cast<unsigned int>(pos))));
            if (str == "-o" || str == "-c")
            {
                ++pos;
                continue;
            }
            if (str.substr(0, 2) == "-I")
            {
                local_include_dirs.emplace(str);
                continue;
            }
            if (str == "-isystem")
            {
                // read the next argument and increment the index in same time
                std::string dir_path(to_string(clang_CompileCommand_getArg(
                    command, static_cast<unsigned int>(++pos))));
                // TODO try std::move on emplace
                system_include_dirs.emplace(dir_path);
                continue;
            }
        }
    }

    // free compile commands
    clang_CompileCommands_dispose(all_compile_commands);

    // TODO try std::move on pus_back
    for (const auto &include_dir : system_include_dirs)
    {
        m_flags.push_back("-isystem");
        m_flags.push_back(include_dir);
    }
    for (const auto &include_dir : local_include_dirs)
    {
        m_flags.push_back(include_dir);
    }
}

std::vector<std::string> Parser_Impl::get_all_filenames()
{

    // all files associated to the compile commands
    std::vector<std::string> files;
    auto                     all_compile_commands =
        clang_CompilationDatabase_getAllCompileCommands(m_db);
    auto size = clang_CompileCommands_getSize(all_compile_commands);
    // parse each file
    for (unsigned i = 0; i < size; ++i)
    {
        auto command =
            clang_CompileCommands_getCommand(all_compile_commands, i);
        auto filename = to_string(clang_CompileCommand_getFilename(command));
        files.push_back(filename);
    }
    return files;
}

} // namespace analyzer
} // namespace code
