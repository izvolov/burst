#ifndef BURST_TUPLE_EACH_HPP
#define BURST_TUPLE_EACH_HPP

#include <burst/functional/compose.hpp>

#include <type_traits>
#include <utility>

namespace burst
{
    //!     Применение функции к множеству объектов
    /*!
            Своего рода "сигнал" для функции композиции, с помощью которого можно указать, что
        композиция должна происходить несколько иначе, чем обычно (см. `each_compose_fn`).
     */
    template <typename UnaryFunction>
    struct each_fn
    {
        UnaryFunction f;
    };

    //!     Поэлементная композиция
    /*!
            Принимает произвольный набор аргументов. К каждому аргументу применяется запомненная
        ранее функция `R`, а затем этот набор отображённых аргументов передаётся в функцию 'L',
        также запомненную ранее.
            Иными словами, входные аргументы "прокидываются" в функцию 'L', при этом каждый из них
        попутно преобразовывается при помощи функции 'R'.
     */
    template <typename L, typename R>
    struct each_compose_fn
    {
        template <typename ... Ts>
        constexpr decltype(auto) operator () (Ts && ... ts) const &
        {
            return l(r(std::forward<Ts>(ts))...);
        }

        template <typename ... Ts>
        constexpr decltype(auto) operator () (Ts && ... ts) &
        {
            return l(r(std::forward<Ts>(ts))...);
        }

        template <typename ... Ts>
        constexpr decltype(auto) operator () (Ts && ... ts) &&
        {
            return std::move(l)(r(std::forward<Ts>(ts))...);
        }

        L l;
        R r;
    };

    template <typename UnaryFunction>
    constexpr auto each (UnaryFunction && f)
        -> each_fn<std::decay_t<UnaryFunction>>
    {
        return {std::forward<UnaryFunction>(f)};
    }

    //!     Композиция двух поэлементных преобразований
    /*!
            Возвращает новое поэлементное преобразование, созданное из композиции внутренних
        функций входных поэлементых преобразований.
     */
    template <typename UnaryFunction1, typename UnaryFunction2>
    constexpr auto compose (each_fn<UnaryFunction1> l, each_fn<UnaryFunction2> r)
        -> each_fn<decltype(compose(l.f, r.f))>
    {
        return {compose(std::move(l.f), std::move(r.f))};
    }

    //!     Композиция поэлементного преобразования с обычной функцией
    /*!
            См. `each_compose_fn`.
     */
    template <typename L, typename UnaryFunction>
    constexpr auto compose (L && l, each_fn<UnaryFunction> r)
        -> each_compose_fn<std::decay_t<L>, UnaryFunction>
    {
        return {std::forward<L>(l), std::move(r.f)};
    }

    template <typename UnaryFunction, typename R>
    constexpr auto operator | (each_fn<UnaryFunction> l, R && r)
    {
        return compose(std::forward<R>(r), std::move(l));
    }
} // namespace burst

#endif // BURST_TUPLE_EACH_HPP
