#include "compile_command.hpp"

#include <catch.hpp>
#include <fstream>
#include <iostream>

#include <filesystem>

TEST_CASE("equal", "[compile_command]")
{
    auto directory{"/tmp/code-analyzer/build"};
    auto file{"/tmp/code-analyzer/out.cpp"};

    // create directory
    std::filesystem::create_directories(directory);
    {
        // create file
        std::ofstream out(file);
    }
    code::analyzer::compile_command cc1(directory,
                                        "c++ -std=c++17 -NDEBUG -o "
                                        "/tmp/code-analyzer/build/out.o -c "
                                        "/tmp/code-analyzer/out.cpp",
                                        "/tmp/code-analyzer/out.cpp");
    code::analyzer::compile_command cc2(directory,
                                        "c++ -std=c++17 -NDEBUG -o "
                                        "/tmp/code-analyzer/build/out.o -c "
                                        "/tmp/code-analyzer/out.cpp",
                                        file);

    REQUIRE(cc1 == cc2);

    code::analyzer::compile_command cc3(directory,
                                        "c++ -std=c++17 -NDEBUG -SHARED -o "
                                        "/tmp/code-analyzer/build/out.o -c "
                                        "/tmp/code-analyzer/out.cpp",
                                        file);

    REQUIRE(cc1 != cc3);
}
