#ifndef BURST_FUNCTIONAL_TRIVIAL_WRITE_HPP
#define BURST_FUNCTIONAL_TRIVIAL_WRITE_HPP

#include <ostream>
#include <type_traits>

namespace burst
{
    struct trivial_write_fn
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
    constexpr auto trivial_write = trivial_write_fn{};
} // namespace burst

#endif // BURST_FUNCTIONAL_TRIVIAL_WRITE_HPP
