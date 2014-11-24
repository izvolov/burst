#ifndef THRUST_ALGORITHM_SEARCHING_DETAIL_BITMASK_TRAITS_HPP
#define THRUST_ALGORITHM_SEARCHING_DETAIL_BITMASK_TRAITS_HPP

#include <bitset>
#include <type_traits>
#include <climits>

#define TYPE_IS_NOT_INTEGRAL_ERROR_MESSAGE \
"Тип битовой маски не является целочисленным. \
Создайте специализацию класса bitmask_traits для своего класса."

namespace thrust
{
    namespace algorithm
    {
        namespace detail
        {
            template <typename Bitmask>
            struct bitmask_traits
            {
                static_assert(std::is_integral<Bitmask>::value, TYPE_IS_NOT_INTEGRAL_ERROR_MESSAGE);
                static const std::size_t size = sizeof(Bitmask) * CHAR_BIT;
            };

            template <std::size_t Bits>
            struct bitmask_traits <std::bitset<Bits>>
            {
                static const std::size_t size = Bits;
            };
        } // namespace detail
    } // namespace algorithm
} // namespace thrust

#endif // THRUST_ALGORITHM_SEARCHING_DETAIL_BITMASK_TRAITS_HPP
