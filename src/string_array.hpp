#include <string>
#include <iostream>

struct string_array
{
    string_array(const std::string &str)
    {
        auto v = split(str);
        m_sarray = new char*[v.size()];
        for(std::size_t i = 0; i < v.size(); ++i)
        {
            m_sarray[i] = const_cast<char*>(v[i].c_str());
            ++m_size;
        }
    }

    ~string_array()
    {
        delete m_sarray;
    }

    size_t size()
    {
        return m_size;
    }

    char** data()
    {
        return m_sarray;
    }

    private:
        std::vector<std::string> split(const std::string &str)
        {
            auto sep = ' ';
            std::vector<std::string> v;
            std::string s;
            for(auto i  = std::begin(str); i != std::end(str); ++i)
            {
                if( *i == sep )
                {
                    v.push_back(s);
                    s.clear();
                }
                else
                {
                    s+=*i;
                }
            }
            v.push_back(std::move(s));
            return v;
        }

        char** m_sarray = nullptr;
        std::size_t m_size = 0;
};
