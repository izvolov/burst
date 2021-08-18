#ifndef BURST__FUNCTIONAL__NOT_FN_HPP
#define BURST__FUNCTIONAL__NOT_FN_HPP

#include <burst/functional/invoke.hpp>

#include <type_traits>
#include <utility>

namespace burst
{
    template <typename Predicate>
    struct not_fn_t
    {
        template <typename ... As>
        constexpr bool operator () (As && ... as) const &
        {
            return not invoke(f, std::forward<As>(as)...);
        }

        template <typename ... As>
        constexpr bool operator () (As && ... as) &
        {
            return not invoke(f, std::forward<As>(as)...);
        }

        template <typename ... As>
        constexpr bool operator () (As && ... as) &&
        {
            return not invoke(std::move(f), std::forward<As>(as)...);
        }

        Predicate f;
    };

    template <typename Predicate>
    auto not_fn (Predicate && f)
        -> not_fn_t<std::decay_t<Predicate>>
    {
        return {std::forward<Predicate>(f)};
    }
}

#endif // BURST__FUNCTIONAL__NOT_FN_HPP
