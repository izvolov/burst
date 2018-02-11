#ifndef BURST_FUNCTIONAL_FUNC_HPP
#define BURST_FUNCTIONAL_FUNC_HPP

#include <burst/functional/compose.hpp>
#include <burst/functional/invoke.hpp>

#include <type_traits>
#include <utility>

namespace burst
{
    //!     Функциональный объект
    /*!
            Обёртка вокруг любого функционального объекта, обеспечивающая его компонуемость.
     */
    template <typename F>
    struct func
    {
        template <typename ... As>
        constexpr decltype(auto) operator () (As && ... as) const &
        {
            return invoke(f, std::forward<As>(as)...);
        }

        template <typename ... As>
        constexpr decltype(auto) operator () (As && ... as) &
        {
            return invoke(f, std::forward<As>(as)...);
        }

        template <typename ... As>
        constexpr decltype(auto) operator () (As && ... as) &&
        {
            return invoke(std::move(f), std::forward<As>(as)...);
        }

        F f;
    };

    template <typename F>
    constexpr auto make_func (F && f)
        -> func<std::decay_t<F>>
    {
        return {std::forward<F>(f)};
    }

    template <typename F, typename R>
    constexpr auto operator | (func<F> l, R && r)
    {
        return compose(std::forward<R>(r), std::move(l));
    }
} // namespace burst

#endif // BURST_FUNCTIONAL_FUNC_HPP
