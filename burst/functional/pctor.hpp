#ifndef BURST_FUNCTIONAL_PCTOR_HPP
#define BURST_FUNCTIONAL_PCTOR_HPP

#include <utility>

namespace burst
{
    //!     Круглоскобочный контруктор
    /*!
            pctor = parentheses + constructor
     */
    template <typename T>
    struct pctor_fn
    {
        template <typename ... As>
        constexpr auto operator () (As && ... as) const
        {
            return T(std::forward<As>(as)...);
        }
    };

    template <typename T>
    constexpr auto pctor = pctor_fn<T>{};
}

#endif // BURST_FUNCTIONAL_PCTOR_HPP
