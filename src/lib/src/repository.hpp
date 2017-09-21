#pragma once

#include <set>
#include <string>

template <class T>
struct usr_t
{
    T           name;
    std::set<T> definitions;
};

template <class T> class repository
{
  public:
    repository() = default;
    ~repository() = default;
    repository(const Repository &) = default;
    repository(Repository &&) = default;
    repository & operator=(const Respository &) = default;
    repository & operator=(Respository &&) = default;

    void save(const T &data)
    {
      auto value = data.name;
      for(auto &e : data.definitions)
      {
        m_definititions[e].emplace(value);
      }
    }

    std::set<T> usr_definitions(const std::string &key)
    {
        std::set<T> ret;
        return ret;
    }

  private:
    std::unordered_map<T, std::set<T>> m_definitions;
};
