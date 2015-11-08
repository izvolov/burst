#ifndef BURST_ALGORITHM_SORTING_DETAIL_TO_UNSIGNED_HPP
#define BURST_ALGORITHM_SORTING_DETAIL_TO_UNSIGNED_HPP

namespace burst
{
    namespace detail
    {
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
            auto operator () (Value && value) const ->
                typename std::enable_if
                <
                    std::is_signed<typename std::decay<typename std::result_of<Map(Value)>::type>::type>::value,
                    typename std::make_unsigned<typename std::decay<typename std::result_of<Map(Value)>::type>::type>::type
                >
                ::type
            {
                using integer_type = typename std::decay<typename std::result_of<Map(Value)>::type>::type;
                constexpr static const auto min_value = std::numeric_limits<integer_type>::min();

                return static_cast<typename std::make_unsigned<integer_type>::type>(map(std::forward<Value>(value)) - min_value);
            }

            template <typename Value>
            auto operator () (Value && value) const ->
                typename std::enable_if
                <
                    std::is_unsigned<typename std::decay<typename std::result_of<Map(Value)>::type>::type>::value,
                    typename std::result_of<Map(Value)>::type
                >
                ::type
            {
                return map(std::forward<Value>(value));
            }

            Map map;
        };

        template <typename Map>
        to_unsigned_t<Map> to_unsigned (Map && map)
        {
            return to_unsigned_t<Map>{std::forward<Map>(map)};
        }
    } // namespace detail
} // namespace burst

#endif // BURST_ALGORITHM_SORTING_DETAIL_TO_UNSIGNED_HPP
