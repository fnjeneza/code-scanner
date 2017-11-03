#pragma once

#include <algorithm>
#include <fstream>
#include <mutex>
#include <set>
#include <unordered_map>

#include "file.hpp"
#include "serializer.hpp"

namespace code::analyzer {
template <class T, class Container = std::set<T>>
class repository
{
  public:
    repository() { deserialize(); };
    ~repository() { serialize(); };
    repository(const repository &) = default;
    repository(repository &&)      = default;
    repository &operator=(const repository &) = default;
    repository &operator=(repository &&) = default;

    // emplace definitions in the repository database
    void emplace(const T &filename, const Container &definitions)
    {
        for (auto &e : definitions)
        {
            {
                // lock in case of multithreaded access
                std::unique_lock<std::mutex> lock(m_mutex);
                m_database[e].emplace(filename);
            }
        }

        utils::File file(filename);
        {
            std::unique_lock<std::mutex> lock(m_mutex);
            auto                         response = m_files.insert(file);
            // insertion fail if the key already exist
            if (!response.second)
            {
                // update only the timestamp
                response.first->set_timestamp(file.timestamp());
            }
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

    // serialize the database to a file
    void serialize() { m_serializer.serialize(m_database, m_files); }

    // deserialize the database from a file
    void deserialize() { m_serializer.deserialize(m_database, m_files); }

    // Check if a file has been processed based on its timestamp.
    // If timestamp is less than the current given in argument  or timestamp
    // not present, add it to the returned container
    template<typename Sequence>
    Sequence check_file_timestamp(const Sequence &filenames)
    {
        // compare each filename from filenames within the filename stored in
        // m_files. If timestamp is different add the filename in the vector to
        // return
        Sequence __ret;
        for (auto &file : filenames)
        {
            utils::File __current(file);
            auto        it = std::find_if(
                std::cbegin(m_files), std::cend(m_files), [&](auto &__stored) {
                    return __stored.path() == file;
                });
            if (it != std::cend(m_files))
            {
                if (it->timestamp() < __current.timestamp())
                {
                    // if the stored parse date is older than the current one,
                    // add it to return container in order to be parsed
                    __ret.emplace_back(file);
                }
            }
            else
            {
                // if the file is not found in the container, add it in the
                // container to return in order to be parsed
                __ret.emplace_back(file);
            }
        }
        return __ret;
    }

  private:
    // stores [usr string, set of filenames]
    std::unordered_map<T, Container> m_database{};
    // stores file and timestamp
    std::set<utils::File> m_files{};
    serializer            m_serializer{};
    std::mutex            m_mutex;
};
} // namespace code::analyzer
