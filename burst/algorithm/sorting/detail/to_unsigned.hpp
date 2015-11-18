#ifndef BURST_ALGORITHM_SORTING_DETAIL_TO_UNSIGNED_HPP
#define BURST_ALGORITHM_SORTING_DETAIL_TO_UNSIGNED_HPP

namespace burst
{
    namespace detail
    {
        //!     Преобразовать целочисленный тип к беззнаковому.
        /*!
                std::make_unsigned не работает с типом bool.
         */
        template <typename T>
        struct make_unsigned_even_bool
        {
            using type = typename std::make_unsigned<T>::type;
        };

        template <>
        struct make_unsigned_even_bool <bool>
        {
            using type = bool;
        };

        //!     Преобразование результата вызова к беззнаковому типу.
        /*!
                Функциональный объект, принимающий некоторое значение, применяющий к нему заданную
            операцию (которая должна возвращать целое число) и преобразующий её результат в
            беззнаковое целое аналогичного размера.

                Принцип преобразования следующий:
                1. Если полученное число беззнаковое, то с ним ничего не делается.
                2. Если знаковое, то из него вычитается его минимальное значение.
                   Иначе говоря, если его изначальный диапазон был [-x, x - 1], то на выходе
                   получается [0, 2x - 1], где x = 2 ^ (количество бит в числе).
         */
        template <typename Map>
        struct to_unsigned_t
        {
            template <typename Value>
            constexpr auto operator () (Value && value) const
            {
                using integer_type = typename std::decay<typename std::result_of<Map(Value)>::type>::type;
                constexpr const auto min_value = std::numeric_limits<integer_type>::min();

                return static_cast<typename make_unsigned_even_bool<integer_type>::type>(map(std::forward<Value>(value)) ^ min_value);
            }

            Map map;
        };

        template <typename Map>
        constexpr to_unsigned_t<Map> to_unsigned (Map && map)
        {
            return to_unsigned_t<Map>{std::forward<Map>(map)};
        }
    } // namespace detail
} // namespace burst

#endif // BURST_ALGORITHM_SORTING_DETAIL_TO_UNSIGNED_HPP
