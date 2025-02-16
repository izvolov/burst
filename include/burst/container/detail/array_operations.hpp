#ifndef BURST__CONTAINER__DETAIL__ARRAY_OPERATIONS_HPP
#define BURST__CONTAINER__DETAIL__ARRAY_OPERATIONS_HPP

#include <algorithm>
#include <array>
#include <cstddef>
#include <iterator>

namespace burst
{
    namespace detail
    {
        template <std::size_t From, std::size_t To, typename Value, std::size_t Size>
        constexpr auto subarray (const std::array<Value, Size> & a)
        {
            static_assert(From <= Size, "Правая граница подмассива вылезает за сам массив");
            static_assert(To <= Size, "Левая граница подмассива вылезает за сам массив");
            static_assert(From <= To, "Левая граница подмассива правее правой границы");

            std::array<Value, To - From> b{};
            std::copy(std::next(std::begin(a), From), std::next(std::begin(a), To), std::begin(b));
            return b;
        }

        template <typename NewInteger, typename OldInteger, std::size_t Size>
        constexpr auto cast (const std::array<OldInteger, Size> & a)
        {
            std::array<NewInteger, Size> b{};
            std::transform(a.begin(), a.end(), b.begin(),
                [] (auto x)
                {
                    return static_cast<NewInteger>(x);
                });
            return b;
        }
    } // namespace detail
} // namespace burst

#endif // BURST__CONTAINER__DETAIL__ARRAY_OPERATIONS_HPP
