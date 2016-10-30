#ifndef BURST_FUNCTIONAL_BCTOR_HPP
#define BURST_FUNCTIONAL_BCTOR_HPP

#include <utility>

namespace burst
{
    //!     Фигурноскобочный контруктор
    /*!
            bctor = braces + constructor
     */
    template <typename T>
    struct bctor_fn
    {
        template <typename ... As>
        constexpr auto operator () (As && ... as) const
        {
            return T{std::forward<As>(as)...};
        }
    };

    template <typename T>
    constexpr auto bctor = bctor_fn<T>{};
}

#endif // BURST_FUNCTIONAL_BCTOR_HPP
