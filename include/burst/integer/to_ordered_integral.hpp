#ifndef BURST__INTEGER__TO_ORDERED_INTEGRAL_HPP
#define BURST__INTEGER__TO_ORDERED_INTEGRAL_HPP

#include <burst/integer/detail/to_ordered_integral.hpp>

namespace burst
{
    /*!
        \brief
            Представить скалярный тип в виде упорядоченного целого

        \details
            Функция определена для упорядоченных скалярных типов, то есть для целых чисел,
            чисел с плавающей запятой, указателей и перечислений.

            Возвращает такое целочисленное представление, что:
                ∀ x, y: x < y => to_ordered_integral(x) < to_ordered_integral(y),
            где `x`, `y` — значения типа `Scalar`.
     */
    struct to_ordered_integral_fn
    {
        template <typename Scalar>
        constexpr auto operator () (Scalar value) const
        {
            static_assert(std::is_scalar<Scalar>::value, "Значение должно быть скаляром");
            return detail::to_ordered_integral_impl(value);
        }
    };

    constexpr auto to_ordered_integral = to_ordered_integral_fn{};
} // namespace burst

#endif // BURST__INTEGER__TO_ORDERED_INTEGRAL_HPP
