#ifndef BURST__STRING__U8S_HPP
#define BURST__STRING__U8S_HPP

#include <cstddef>
#include <string>

namespace burst
{
    namespace literals
    {
        inline std::string operator ""_u8s (const char * str, std::size_t len)
        {
            return std::string(str, len);
        }

#if defined(__cpp_lib_char8_t)
        inline std::string operator ""_u8s (const char8_t * str, std::size_t len)
        {
            auto u8s = std::basic_string<char8_t>(str, len);
            return std::string(u8s.begin(), u8s.end());
        }
#endif
    }
}

#endif // BURST__STRING__U8S_HPP
