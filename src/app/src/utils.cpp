#include "utils.hpp"

namespace utils
{
    std::vector<std::string> split(const std::string & argument)
    {
        std::vector<std::string> ret;
        unsigned start = 0;
        while(true)
        {
            std::size_t found = argument.find(' ', start);
            if(found == std::string::npos)
            {
                ret.push_back(argument.substr(start));
                break;
            }
            ret.push_back(argument.substr(start, found-start));
            start = found+1;
        }
        return ret;
    }
}
