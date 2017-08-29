#include "args.hxx"
#include "parser.hpp"
#include <experimental/filesystem>
#include <iostream>

namespace std {
namespace filesystem = std::experimental::filesystem;
}

int main(int argc, char **argv)
{
    // --compile_commands_dir=<path> path to compile commands directory
    // -f <filename to handle>
    // -r row
    // -c column
    args::ArgumentParser arg("icscope program");
    args::HelpFlag help(arg, "help", "Display this help menu", {'h', "help"});
    args::ValueFlag<std::string> compile_commands(
        arg, "compile_commands_dir", "compile commands directory", {"cc"});
    args::ValueFlag<std::string> file(
        arg, "file", "source/header file path", {'f', "file"});
    args::ValueFlag<int> row(arg, "row", "The line number", {'l'});
    args::ValueFlag<int> col(arg, "offset", "The column number", {'o'});

    args::Group group(arg, "Find this symbol", args::Group::Validators::Xor);
    args::Flag  g(group, "definition", "Find this symbol definition", {'g'});
    args::Flag  c(
        group, "callers", "FInd functions calling this function", {'c'});
    args::Flag s(group, "reference", "Find symbol reference", {'s'});
    try
    {
        arg.ParseCLI(argc, argv);
    }
    catch (args::Help)
    {
        std::cout << arg;
        return 0;
    }
    catch (args::ParseError e)
    {
        std::cerr << e.what() << std::endl;
        std::cerr << arg;
        return 1;
    }

    std::filesystem::path build_path;
    std::filesystem::path filename;
    int                   line;
    int                   column;

    if (compile_commands)
    {
        build_path = args::get(compile_commands);
    }
    if (file)
    {
        filename = args::get(file);
    }
    if (row)
    {
        line = args::get(row);
    }
    if (col)
    {
        column = args::get(col);
    }

    code::analyzer::Parser parser(build_path, filename);
    std::cout << "##############\n";
    auto cursor = parser.cursor(line, column);
    if (c)
    {
        auto cursors = parser.callers(cursor);

        std::cout << "[";
        for (size_t i = 0; i < cursors.size(); ++i)
        {
            auto loc = code::analyzer::location(cursors[i]);
            std::cout << "{"
                      << "\"file\":\"" << std::get<0>(loc)
                      << "\",\"line\":" << std::get<1>(loc)
                      << ",\"col\":" << std::get<2>(loc) << "}";
            if (i < cursors.size() - 1)
            {
                std::cout << ",\n";
            }
            else
            {
                std::cout << "\n";
            }
        }
        std::cout << "]\n";
        return 0;
    }
    if (g)
    {
        cursor = code::analyzer::definition(cursor);
    }
    if (s)
    {
        cursor = code::analyzer::declaration(cursor);
    }
    auto loc = code::analyzer::location(cursor);
    std::cout << "{"
              << "\"file\":\"" << std::get<0>(loc)
              << "\",\"line\":" << std::get<1>(loc)
              << ",\"col\":" << std::get<2>(loc) << "}\n";

    return 0;
}
