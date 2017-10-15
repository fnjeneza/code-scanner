#include "code-scanner/Params.hpp"
#include "code-scanner/code-scanner.hpp"
#include <args.hxx>
#include <fstream>
#include <iostream>

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
    args::ValueFlag<std::string> config_file(
        arg, "config_file", "json config file", {'c', "config"});
    args::ValueFlag<unsigned int> row(arg, "row", "The line number", {'l'});
    args::ValueFlag<unsigned int> col(
        arg, "offset", "The column number", {'o'});
    args::Group group(arg, "Find this symbol", args::Group::Validators::Xor);
    args::Flag  g(group, "definition", "Find this symbol definition", {'g'});
    args::Flag  c(
        group, "callers", "Find functions calling this function", {'c'});
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
    catch (args::ValidationError e)
    {
        std::cerr << e.what() << std::endl;
        std::cerr << arg;
        return 1;
    }

    std::string  build_path;
    std::string  filename;
    std::string  configuration_file = "config.json";
    unsigned int line;
    unsigned int column;

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
    if (config_file)
    {
        configuration_file = args::get(config_file);
    }

    // Initialize parameters
    code::analyzer::InitializeParams initialize_params;
    initialize_params.rootUri = build_path;

    // The config file where options are specified
    initialize_params.initializationOptions = configuration_file;

    code::analyzer::Parser parser;
    parser.initialize(initialize_params);

    // code::analyzer::Parser parser(build_path, filename, compile_arguments);
    // auto cursor = parser.cursor(line, column);
    // if (c)
    // {
    //     auto cursors = parser.callers(cursor);

    //     std::cout << "[";
    //     for (size_t i = 0; i < cursors.size(); ++i)
    //     {
    //         auto loc = code::analyzer::location(cursors[i]);
    //         std::cout << "{"
    //                   << "\"file\":\"" << std::get<0>(loc)
    //                   << "\",\"line\":" << std::get<1>(loc)
    //                   << ",\"col\":" << std::get<2>(loc) << "}";
    //         if (i < cursors.size() - 1)
    //         {
    //             std::cout << ",\n";
    //         }
    //         else
    //         {
    //             std::cout << "\n";
    //         }
    //     }
    //     std::cout << "]\n";
    //     return 0;
    // }
    if (g)
    {
        code::analyzer::ReferenceParams params;
        params.textDocument.uri = filename;
        code::analyzer::Position position;
        position.line      = line;
        position.character = column;
        params.position    = position;

        code::analyzer::Location location = parser.definition(params);
        std::cout << location.uri << ":" << location.range.start.line << ":"
                  << location.range.start.character << '\n';
    }
    if (s)
    {
        code::analyzer::ReferenceParams params;
        params.textDocument.uri = filename;
        code::analyzer::Position position;
        position.line      = line;
        position.character = column;
        params.position    = position;

        code::analyzer::Location location = parser.references(params);
        std::cout << location.uri << ":" << location.range.start.line << ":"
                  << location.range.start.character << '\n';
    }
    // auto loc = code::analyzer::location(cursor);
    // std::cout << "{"
    //           << "\"file\":\"" << std::get<0>(loc)
    //           << "\",\"line\":" << std::get<1>(loc)
    //           << ",\"col\":" << std::get<2>(loc) << "}\n";

    return 0;
}
