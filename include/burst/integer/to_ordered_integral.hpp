#ifndef BURST__INTEGER__TO_ORDERED_INTEGRAL_HPP
#define BURST__INTEGER__TO_ORDERED_INTEGRAL_HPP

#include <burst/integer/detail/to_ordered_integral.hpp>

#include <type_traits>
#include <limits>

namespace burst
{
    /*!
        \brief
            Представить арифметический тип в виде упорядоченного целого

        \details
            Функция определена для упорядоченных арифметических типов, то есть для целых чисел и
            чисел с плавающей запятой.

            Возвращает такое целочисленное представление, что:
                ∀ x, y: x < y => to_ordered_integral(x) < to_ordered_integral(y),
            где `x`, `y` — значения типа `Arithmetic`.
     */
    struct to_ordered_integral_fn
    {
        template <typename Arithmetic>
        constexpr auto operator () (Arithmetic value) const
        {
            static_assert
            (
                std::is_integral<Arithmetic>::value || std::numeric_limits<Arithmetic>::is_iec559,
                "Требуется целое число или число с плавающей запятой, "
                "соответствующее стандарту IEC 559 (IEEE 754)"
            );
            return detail::to_ordered_integral_impl(value);
        }
    };

    constexpr auto to_ordered_integral = to_ordered_integral_fn{};
} // namespace burst

#endif // BURST__INTEGER__TO_ORDERED_INTEGRAL_HPP
