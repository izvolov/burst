#ifndef BURST__INTEGER__DETAIL__TO_ORDERED_INTEGRAL_HPP
#define BURST__INTEGER__DETAIL__TO_ORDERED_INTEGRAL_HPP

#include <burst/bit/bit_cast.hpp>
#include <burst/type_traits/unsigned_integer_of_size.hpp>

#include <limits>
#include <type_traits>

namespace burst
{
    namespace detail
    {
        template <typename Integral>
        constexpr auto to_ordered_integral_impl (Integral n)
            -> std::enable_if_t<std::is_integral<Integral>::value, Integral>
        {
            return n;
        }

        /*!
            \brief
                Перегрузка для чисел с плавающей запятой

            \details
                Из стандарта IEEE 754 мы знаем, что:
                1.  Битовое представление положительных чисел непосредственно отражает их порядок:
                    При сравнении чисел по модулю число с большим показателем степени окажется
                    больше, а если показатели степени равны, большим окажется то, у которого большая
                    мантисса.
                2.  Битовое представление отрицательных чисел отражает их обратный порядок (из тех
                    же соображений).
                3.  Старший бит равен нулю для положительных чисел и единице для отрицательных чисел.
                    Поэтому в исходном битовом представлении любое отрицательное число будет больше
                    любого положительного.

                Исходя их этих вводных, чтобы получить упорядоченное битовое представление чисел с
                плавающей запятой, нужно:
                1.  Инвертировать битовое представление (включая знаковый бит) отрицательных чисел,
                    чтобы перейти от обратного порядка к прямому;
                2.  Инвертировать знаковый бит для положительных чисел.

                Таким образом, мы развернули порядок на отрицательных числах и все отрицательные
                числа сделали меньше всех положительных (за счёт инвертирования знакового бита).

                Подробнее: https://www.rsdn.org/article/alg/float.xml#E3NAE
         */
        template <typename Floating>
        constexpr auto to_ordered_integral_impl (Floating f)
            ->
                std::enable_if_t
                <
                    std::numeric_limits<Floating>::is_iec559,
                    unsigned_integer_of_size_t<sizeof(Floating)>
                >
        {
            using integral_type = unsigned_integer_of_size_t<sizeof(Floating)>;
            constexpr auto bit_count = std::numeric_limits<integral_type>::digits;
            constexpr auto sign_bit_mask =
                static_cast<integral_type>(integral_type{1} << (bit_count - 1));

            const auto u = bit_cast<integral_type>(f);

            return static_cast<integral_type>((u & sign_bit_mask) ? ~u : u ^ sign_bit_mask);
        }
    } // namespace detail
} // namespace burst

#endif // BURST__INTEGER__DETAIL__TO_ORDERED_INTEGRAL_HPP
