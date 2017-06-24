#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include "parser.hpp"

TEST_CASE("Locate cursor", "[location]")
{
    std::string filename("data/index.cpp");
    code::analyzer::Parser parser(filename);
    unsigned long line =  11;
    unsigned long column = 10;
    CXCursor cursor  = parser.cursor(line, column);

    auto loc = code::analyzer::location(cursor);
    auto filename_ret = std::get<0>(loc);
    auto line_ret = std::get<1>(loc);
    auto column_ret = std::get<2>(loc);
    REQUIRE(filename_ret == filename);
    REQUIRE(line_ret == line);
    REQUIRE(column_ret == column);
}
