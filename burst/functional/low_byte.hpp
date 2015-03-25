#ifndef BURST_FUNCTIONAL_LOW_BYTE_HPP
#define BURST_FUNCTIONAL_LOW_BYTE_HPP

#include <type_traits>

namespace burst
{
    //!     Взятие младшего байта из целого числа, параметризованное типом.
    /*!
            Тип результирующего значения зафиксирован — это std::uint8_t. Для одного байта этого
        достаточно.
     */
    template <typename Integer = void>
    struct low_byte
    {
        static_assert(std::is_integral<Integer>::value, "Младший байт можно взять только от целого числа.");

        std::uint8_t operator () (Integer integer) const
        {
            return integer & 0xff;
        }
    };

    //!     Взятие младшего байта из целого числа.
    /*!
            В отличие от аналогичного функционального объекта, параметризованного типом значения,
        не зависит от типа принимаемого значения.
     */
    template <>
    struct low_byte <void>
    {
        template <typename Integer>
        std::uint8_t operator () (Integer integer) const
        {
            static_assert(std::is_integral<Integer>::value, "Младший байт можно взять только от целого числа.");

            return integer & 0xff;
        }
    };
}

#endif // BURST_FUNCTIONAL_LOW_BYTE_HPP
