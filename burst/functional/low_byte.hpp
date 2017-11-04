#ifndef BURST_FUNCTIONAL_LOW_BYTE_HPP
#define BURST_FUNCTIONAL_LOW_BYTE_HPP

#include <cstdint>
#include <type_traits>

namespace burst
{
    //!     Взятие младшего байта из целого числа
    /*!
            Тип результирующего значения зафиксирован — это std::uint8_t. Для одного байта этого
        достаточно.
     */
    struct low_byte_fn
    {
        template <typename Integer>
        constexpr std::uint8_t operator () (Integer integer) const
        {
            static_assert
            (
                std::is_integral<Integer>::value,
                "Младший байт можно взять только от целого числа."
            );

            return static_cast<std::uint8_t>(integer & 0xff);
        }
    };
    constexpr auto low_byte = low_byte_fn{};
}

#endif // BURST_FUNCTIONAL_LOW_BYTE_HPP
