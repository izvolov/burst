#ifndef BURST__CONTAINER__ACCESS__BACK_HPP
#define BURST__CONTAINER__ACCESS__BACK_HPP

#include <cstddef>

namespace burst
{
    struct back_t
    {
        template <typename T, std::size_t Size>
        constexpr decltype(auto) operator () (T (& array) [Size]) const
        {
            return array[Size - 1];
        }

        template <typename Container>
        constexpr decltype(auto) operator () (Container & container) const
        {
            return container.back();
        }
    };

    constexpr auto back = back_t{};
} // namespace burst

#endif // BURST__CONTAINER__ACCESS__BACK_HPP
