#pragma once

#include <set>
#include <unordered_map>

template <class T, class Container=std::set<T>> class repository
{
  public:
    repository()                   = default;
    ~repository()                  = default;
    repository(const repository &) = default;
    repository(repository &&)      = default;
    repository &operator=(const repository &) = default;
    repository &operator=(repository &&) = default;

    // Save definitions in the repository database
    void save(const T & filename, const Container & definitions)
    {
        for (auto &e : definitions)
        {
            m_database[e].emplace(filename);
        }
    }

    // retrieve all filenames in which key is defined
    Container definitions(const T &key)
    {
        auto it = m_database.find(key);
        if (it != std::end(m_database))
        {
            return it->second;
        }

        // empty container
        return Container();
    }

  private:
    // stores [usr string, set of filenames]
    std::unordered_map<T, Container> m_database;
};
