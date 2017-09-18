#pragma once

#include <set>
#include <string>

struct usr_t
{
    std::string           m_name;
    std::set<std::string> definitions;
};

template <class T> class Repository
{
  public:
    ~Repository() = default;
    static void save(const T &data) {}
    static std::set<T> usr_definitions(const std::string &key);
    {
        std::set<T> ret;
        return ret;
    }

  private:
    Repository() = default;

    Repository &m_instance;
    std::set<T> m_definitions;
};
