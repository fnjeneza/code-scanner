#pragma once

#include "code-scanner/Location.hpp"
#include "hash_combine.hpp"
#include <string>

namespace code::analyzer {
enum class kind
{
    definition,
    reference
};

struct symbol
{
    symbol()  = default;
    symbol(const std::string_view &usr,
           const Location &        location,
           const kind &            _kind)
        : m_usr{usr}
        , m_location{location}
        , m_kind{_kind}
    {
    }
    symbol(symbol &&)      = default;
    symbol(const symbol &) = default;
    ~symbol() = default;

    friend bool operator==(const symbol &lhs, const symbol &rhs)
    {
        return (lhs.m_usr == rhs.m_usr && lhs.m_location == rhs.m_location &&
                lhs.m_kind == rhs.m_kind);
    }

    std::string      m_usr; // unified symbol resolution
    Location         m_location;
    kind             m_kind = kind::reference;
};

} // namespace code::analyzer

namespace std {
template <>
struct hash<code::analyzer::symbol>
{
    std::size_t operator()(const code::analyzer::symbol &s) const noexcept
    {
        std::size_t seed = 0;
        code::analyzer::hash_combine(seed, s.m_usr);
        code::analyzer::hash_combine(seed, s.m_location.uri);
        code::analyzer::hash_combine(seed, s.m_location.range.start.line);
        code::analyzer::hash_combine(seed, s.m_location.range.start.character);
        code::analyzer::hash_combine(seed, s.m_location.range.end.line);
        code::analyzer::hash_combine(seed, s.m_location.range.end.character);
        code::analyzer::hash_combine(seed, s.m_kind);
        return seed;
    }
};

template <>
struct less<code::analyzer::symbol>
{
    using T = code::analyzer::symbol;
    bool operator()(const T &lhs, const T &rhs)
    {
        return (
            lhs.m_usr < rhs.m_usr || lhs.m_location.uri < rhs.m_location.uri ||
            lhs.m_location.range.start.line < rhs.m_location.range.start.line ||
            lhs.m_location.range.start.character <
                rhs.m_location.range.start.character ||
            lhs.m_kind < rhs.m_kind);
    }
};
} // namespace std
