#ifndef BURST__FUNCTIONAL__TUPLESS_HPP
#define BURST__FUNCTIONAL__TUPLESS_HPP

#include <burst/functional/compose.hpp>
#include <burst/tuple/apply.hpp>
#include <burst/tuple/forward_as_tuple.hpp>

#include <type_traits>
#include <utility>

namespace burst
{
    //!     Контейнер-тег для бескортежной функции
    /*!
            Хранит функцию, которая применяется к кортежу и возвращает другой кортеж.
            Выполняет роль "сигнала" для функции композиции о том, что композиция должна
        происходить иначе, чем обычно (см. ниже).
     */
    template <typename Function>
    struct tupless_fn
    {
        Function f;
    };

    template <typename Function>
    constexpr auto tupless (Function && f)
        -> tupless_fn<std::decay_t<Function>>
    {
        return {std::forward<Function>(f)};
    }

    //!     Композиция двух бескортежных преобразований
    /*!
            Возвращает новое бескортежное преобразование, созданное из композиции внутренних
        функций входных бескортежных преобразований.
     */
    template <typename Function1, typename Function2>
    constexpr auto compose (tupless_fn<Function1> l, tupless_fn<Function2> r)
    {
        return tupless(compose(std::move(l.f), std::move(r.f)));
    }

    //!     Композиция бескортежного преобразования с обычной функцией
    /*!
            Возвращает функциональный объект, эквивалентный по смыслу результату выражения

                forward_as_tuple | r | apply(l)
     */
    template <typename L, typename Function>
    constexpr auto compose (L && l, tupless_fn<Function> r)
    {
        return compose(apply(std::forward<L>(l)), compose(std::move(r.f), forward_as_tuple));
    }

    template <typename Function, typename R>
    constexpr auto operator | (tupless_fn<Function> l, R && r)
    {
        return compose(std::forward<R>(r), std::move(l));
    }
} // namespace burst

#endif // BURST__FUNCTIONAL__TUPLESS_HPP
