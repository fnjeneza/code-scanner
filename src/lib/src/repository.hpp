#pragma once

#include <set>
#include <unordered_map>

template <class T> class repository
{
  public:
    repository()                   = default;
    ~repository()                  = default;
    repository(const repository &) = default;
    repository(repository &&)      = default;
    repository &operator=(const repository &) = default;
    repository &operator=(repository &&) = default;

    void save(const T & filename, const std::set<T> &definitions)
    {
        for (auto &e : definitions)
        {
            m_definitions[e].emplace(filename);
        }
    }

    std::set<T> usr_definitions(const T &key)
    {
        auto it = m_definitions.find(key);
        if (it != std::end(m_definitions))
        {
            return it->second;
        }

        std::set<T> ret;
        return ret;
    }

  private:
    std::unordered_map<T, std::set<T>> m_definitions;
};
