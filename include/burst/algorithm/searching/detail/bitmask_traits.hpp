#ifndef BURST_ALGORITHM_SEARCHING_DETAIL_BITMASK_TRAITS_HPP
#define BURST_ALGORITHM_SEARCHING_DETAIL_BITMASK_TRAITS_HPP

#include <bitset>
#include <climits>
#include <cstddef>
#include <type_traits>

#define TYPE_IS_NOT_INTEGRAL_ERROR_MESSAGE \
"Тип битовой маски не является целочисленным. \
Создайте специализацию класса bitmask_traits для своего класса."

namespace burst
{
    namespace algorithm
    {
        namespace detail
        {
            //!     Характеристики битовой маски.
            /*!
                    Метафункция для выяснения информации о типе битовой маски.
                    Сожержит поле "size", значение которого равно количеству битов в битовой маске.
             */
            template <typename Bitmask>
            struct bitmask_traits
            {
                static_assert(std::is_integral<Bitmask>::value, TYPE_IS_NOT_INTEGRAL_ERROR_MESSAGE);
                static const std::size_t size = sizeof(Bitmask) * CHAR_BIT;
            };

            //!     Характеристики битовой маски типа std::bitset<S>.
            /*!
                    Специализация характеристик битовой маски для класса std::bitset<S>.
                    Содержит все те же поля, что и общая реализация этой метафукции.
             */
            template <std::size_t Bits>
            struct bitmask_traits <std::bitset<Bits>>
            {
                static const std::size_t size = Bits;
            };
        } // namespace detail
    } // namespace algorithm
} // namespace burst

#endif // BURST_ALGORITHM_SEARCHING_DETAIL_BITMASK_TRAITS_HPP
