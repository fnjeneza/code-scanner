#ifndef FILE_HPP
#define FILE_HPP

#include <experimental/filesystem>
#include <string>

namespace std{
namespace filesystem = std::experimental::filesystem;
}

namespace code::analyzer::utils
{

struct File{
    File() = default;
    ~File() = default;

    explicit File(const std::string & path)
    {
        m_path = path;
        auto time = std::filesystem::last_write_time(path);
        m_timestamp = decltype(time)::clock::to_time_t(time);
    }

    std::string path()
    {
        return m_path;
    }

    long int timestamp()
    {
        return m_timestamp;
    }

    std::string m_path;
    long int m_timestamp;
};
}


#endif /* FILE_HPP */
