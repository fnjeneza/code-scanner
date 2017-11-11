#include <string>

enum class kind
{
    definition,
    reference
};

struct symbol
{
    std::string_view m_usr; // unified symbol resolution
    Location         m_location;
    kind             m_kind;
};
