#ifndef BURST_ITERATOR_DETAIL_TRIVIAL_WRITE_HPP
#define BURST_ITERATOR_DETAIL_TRIVIAL_WRITE_HPP

#include <ostream>
#include <type_traits>

namespace burst
{
    namespace detail
    {
        struct trivial_write_t
        {
            template <typename Value>
            std::ostream & operator () (std::ostream & stream, const Value & value) const
            {
                static_assert(std::is_trivial<Value>::value, "");
                return
                    stream.write
                    (
                        reinterpret_cast<const char *>(std::addressof(value)),
                        sizeof(value)
                    );
            }
        };
    } // namespace detail
} // namespace burst

#endif // BURST_ITERATOR_DETAIL_TRIVIAL_WRITE_HPP
