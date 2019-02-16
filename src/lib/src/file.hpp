#pragma once

#include <functional>
#include <string>

#include <filesystem>

namespace code::analyzer::utils {

class File
{
  public:
    File()             = default;
    File(const File &) = default;
    File(File &&)      = default;
    ~File()            = default;

    explicit File(const std::string &path)
        : m_path{path}
    {
        auto time   = std::filesystem::last_write_time(path);
        m_timestamp = decltype(time)::clock::to_time_t(time);
    }

    explicit File(const std::string &path, const long int &timestamp)
        : m_path{path}
        , m_timestamp{timestamp}
    {
    }

    File &operator=(const File &) = default;
    File &operator=(File &&) = default;

    bool operator<(const File &file) const { return m_path < file.m_path; }

    std::string path() const noexcept { return m_path; }

    long int timestamp() const noexcept { return m_timestamp; }

    void set_timestamp(long int timestamp) const { m_timestamp = timestamp; }

  private:
    std::string      m_path{};
    mutable long int m_timestamp = 0;
};
} // namespace code::analyzer::utils
