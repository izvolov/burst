#ifndef BURST_ITERATOR_DETAIL_TRIVIAL_READ_HPP
#define BURST_ITERATOR_DETAIL_TRIVIAL_READ_HPP

#include <istream>
#include <type_traits>

namespace burst
{
    namespace detail
    {
        struct trivial_read_t
        {
            template <typename Value>
            std::istream & operator () (std::istream & stream, Value & value) const
            {
                static_assert(std::is_trivial<Value>::value, "");
                return stream.read(reinterpret_cast<char *>(std::addressof(value)), sizeof(value));
            }
        };
    } // namespace detail
} // namespace burst

#endif // BURST_ITERATOR_DETAIL_TRIVIAL_READ_HPP
