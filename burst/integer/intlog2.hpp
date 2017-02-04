#ifndef BURST_INTEGER_INTLOG2_HPP
#define BURST_INTEGER_INTLOG2_HPP

#include <stdexcept>
#include <type_traits>

namespace burst
{
    namespace detail
    {
        //!     Собственно вычисление двоичного логарифма
        /*!
                Требуется, чтобы входное число было строго положительным.
         */
        template <typename Integer>
        constexpr Integer intlog2_impl (Integer integer)
        {
            auto degree = Integer{0};

            while ((integer >>= 1) > 0)
            {
                ++degree;
            }

            return degree;
        }
    }

    //!     Целая часть двоичного логарифма.
    template <typename Integer>
    constexpr Integer intlog2 (Integer integer)
    {
        static_assert(std::is_integral<Integer>::value, "Число должно быть целым.");

        return integer > 0
            ? detail::intlog2_impl(integer)
            : throw std::domain_error("Двоичный логарифм не определён на неположительных числах.");
    }
}

#endif // BURST_INTEGER_INTLOG2_HPP
