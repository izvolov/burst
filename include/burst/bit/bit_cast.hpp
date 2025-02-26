#ifndef BURST__BIT__BIT_CAST_HPP
#define BURST__BIT__BIT_CAST_HPP

#include <type_traits>

#if defined(__cpp_lib_bit_cast)
#include <bit>
#else
#include <cstring>
#include <memory>
#endif

namespace burst
{
    template <typename To>
    struct bit_cast_fn
    {
        template <typename From>
        constexpr auto operator () (const From & source) const noexcept
            ->
                std::enable_if_t
                <
                    sizeof(To) == sizeof(From) &&
                    std::is_trivially_copyable<From>::value &&
                    std::is_trivially_copyable<To>::value,
                    To
                >
        {
#if defined(__cpp_lib_bit_cast)
            return std::bit_cast<To>(source);
#else
            static_assert(std::is_trivially_constructible<To>::value, "");

            To destination;
            std::memcpy(std::addressof(destination), std::addressof(source), sizeof(To));
            return destination;
#endif
        }
    };

    template <typename To>
    constexpr auto bit_cast = bit_cast_fn<To>{};
} // namespace burst

#endif // BURST__BIT__BIT_CAST_HPP
