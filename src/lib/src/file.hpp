#ifndef FILE_HPP
#define FILE_HPP

#include <experimental/filesystem>
#include <string>

namespace std{
namespace filesystem = std::experimental::filesystem;
}

namespace code::analyzer::utils
{

class File{
    public:
    File() = default;
    ~File() = default;

    explicit File(const std::string & path)
        : m_path{path}
    {
        auto time = std::filesystem::last_write_time(path);
        m_timestamp = decltype(time)::clock::to_time_t(time);
    }

    explicit File(const std::string & path, const long int & timestamp)
        : m_path{path}
        , m_timestamp{timestamp}
    {
    }

    std::string path() const
    {
        return m_path;
    }

    long int timestamp() const
    {
        return m_timestamp;
    }

    private:
    std::string m_path;
    long int m_timestamp;
};
}


#endif /* FILE_HPP */
