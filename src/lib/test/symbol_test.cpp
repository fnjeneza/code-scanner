#include "catch.hpp"
#include "symbol.hpp"

// check produced hash for different values
TEST_CASE("hash symbol", "[symbol_test]")
{
    code::analyzer::symbol s;
    s.m_usr                            = "usr1";
    s.m_location.range.start.line      = 10;
    s.m_location.range.start.character = 100;
    s.m_location.range.end.line        = 10;
    s.m_location.range.end.character   = 120;

    code::analyzer::symbol s2;
    s2.m_usr                            = "usr1";
    s2.m_location.range.start.line      = 10;
    s2.m_location.range.start.character = 100;
    s2.m_location.range.end.line        = 10;
    s2.m_location.range.end.character   = 120;

    auto seed = std::hash<code::analyzer::symbol>{}(s);
    auto seed2 = std::hash<code::analyzer::symbol>{}(s2);

    REQUIRE(seed == seed2);

    auto s3 = s2;
    s3.m_kind  = code::analyzer::kind::definition;
    auto seed3 = std::hash<code::analyzer::symbol>{}(s3);
    REQUIRE(seed != seed3);
}
