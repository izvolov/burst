#ifndef BURST__INTEGER__INTLOG_HPP
#define BURST__INTEGER__INTLOG_HPP

#include <stdexcept>
#include <type_traits>

namespace burst
{
    namespace detail
    {
        template <typename Integer>
        constexpr Integer intlog_impl (Integer number, Integer base)
        {
            auto degree = Integer{0};
            auto exponent = base;

            while (exponent < number)
            {
                exponent *= base;
                ++degree;
            }

            if (exponent == number)
            {
                return degree + 1;
            }
            else
            {
                return degree;
            }
        }
    }
    //!     Целая часть логарифма.
    /*!
            Вычисляет целую часть логарифма числа по заданному основанию.
     */
    template <typename Integer1, typename Integer2>
    constexpr Integer1 intlog (Integer1 number, Integer2 base)
    {
        static_assert(std::is_integral<Integer1>::value, "Число должно быть целым.");
        static_assert(std::is_integral<Integer2>::value, "Основание должно быть целым.");
        using common_type = std::common_type_t<Integer1, Integer2>;

        return
            base > 0
                ? base != 1
                    ? number > 0
                        // Обратное преобразование к типу Integer1 всегда корректно, потому что
                        // результат вычисления всегда строго меньше исходного числа `number`.
                        ? static_cast<Integer1>(detail::intlog_impl
                            (
                                static_cast<common_type>(number),
                                static_cast<common_type>(base)
                            ))
                        : throw std::domain_error("Логарифм не определён на неположительных числах.")
                    : throw std::domain_error("Логарифм по основанию 1 не определён.")
                : throw std::domain_error("Логарифм по неположительному основанию не определён.");
    }
}

#endif // BURST__INTEGER__INTLOG_HPP
