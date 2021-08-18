#ifndef BURST__TUPLE__APPLY_HPP
#define BURST__TUPLE__APPLY_HPP

#include <burst/functional/compose.hpp>
#include <burst/tuple/detail/apply.hpp>

#include <tuple>
#include <type_traits>
#include <utility>

namespace burst
{
    //!     Применение функции к элементам развёрнутого кортежа
    /*!
            Принимает на вход функцию и произвольный кортеж. "Разворачивает" кортеж так, как будто
        его элементы являются отдельными объектами, и применяет к этому множеству объектов заданную
        функцию.
     */
    template <typename NaryFunction, typename Tuple>
    constexpr decltype(auto) apply (NaryFunction && f, Tuple && t)
    {
        return
            detail::apply_impl
            (
                std::forward<NaryFunction>(f),
                std::forward<Tuple>(t),
                std::make_index_sequence<std::tuple_size<std::decay_t<Tuple>>::value>{}
            );
    }

    template <typename NaryFunction>
    struct apply_fn
    {
        template <typename Tuple>
        constexpr decltype(auto) operator () (Tuple && t) const &
        {
            return burst::apply(f, std::forward<Tuple>(t));
        }

        template <typename Tuple>
        constexpr decltype(auto) operator () (Tuple && t) &
        {
            return burst::apply(f, std::forward<Tuple>(t));
        }

        template <typename Tuple>
        constexpr decltype(auto) operator () (Tuple && t) &&
        {
            return burst::apply(std::move(f), std::forward<Tuple>(t));
        }

        NaryFunction f;
    };

    template <typename NaryFunction>
    constexpr auto apply (NaryFunction && f)
        -> apply_fn<std::decay_t<NaryFunction>>
    {
        return {std::forward<NaryFunction>(f)};
    }

    template <typename NaryFunction, typename R>
    constexpr auto operator | (apply_fn<NaryFunction> l, R && r)
    {
        return compose(std::forward<R>(r), std::move(l));
    }
} // namespace burst

#endif // BURST__TUPLE__APPLY_HPP
