#ifndef BURST__FUNCTIONAL__TRIVIAL_READ_HPP
#define BURST__FUNCTIONAL__TRIVIAL_READ_HPP

#include <istream>
#include <memory>
#include <type_traits>

namespace burst
{
    struct trivial_read_fn
    {
        template <typename Value>
        std::istream & operator () (std::istream & stream, Value & value) const
        {
            static_assert(std::is_trivial<Value>::value, "");
            return stream.read(reinterpret_cast<char *>(std::addressof(value)), sizeof(value));
        }
    };
    constexpr auto trivial_read = trivial_read_fn{};
} // namespace burst

#endif // BURST__FUNCTIONAL__TRIVIAL_READ_HPP
