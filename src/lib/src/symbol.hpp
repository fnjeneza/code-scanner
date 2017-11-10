#include <string>

enum class kind
{
    definition,
    reference
};

struct position
{
    std::size_t line;
    std::size_t character;
};

struct range
{
    position start;
    position end;
};

struct symbol
{
    std::string_view m_usr; // unified symbol resolution
    std::string_view m_filename;
    range            m_range;
    kind             m_kind;
};
