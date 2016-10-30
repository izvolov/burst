#ifndef BURST_FUNCTIONAL_COMPOSE_HPP
#define BURST_FUNCTIONAL_COMPOSE_HPP

#include <utility>

namespace burst
{
    //!     Композиция двух отображений
    /*!
            Принимает произвольный набор аргументов и применяет к ним композицию заданных
        отображений `L` и `R` так, что сначала применяется отображение `R`, а затем, к полученному
        результату применяется отображение `L`.
     */
    template <typename L, typename R>
    struct compose_fn
    {
        template <typename ... Ts>
        constexpr decltype(auto) operator () (Ts && ... ts) const &
        {
            return l(r(std::forward<Ts>(ts)...));
        }

        template <typename ... Ts>
        constexpr decltype(auto) operator () (Ts && ... ts) &
        {
            return l(r(std::forward<Ts>(ts)...));
        }

        template <typename ... Ts>
        constexpr decltype(auto) operator () (Ts && ... ts) &&
        {
            return std::move(l)(std::move(r)(std::forward<Ts>(ts)...));
        }

        L l;
        R r;
    };

    template <typename L, typename R>
    constexpr auto compose (L && l, R && r)
        -> compose_fn<std::decay_t<L>, std::decay_t<R>>
    {
        return {std::forward<L>(l), std::forward<R>(r)};
    }

    //!     Оператор для удобного построения композиций
    /*!
            Важно отметить, что данный оператор меняет порядок применения композиции.
            Так, если задана цепочка отображений `a | b | c`, то эти отображения будут применяться
        к входным аргументам слева направо, т.е. `c(b(a(...)))`.
     */
    template <typename ... Ts, typename R>
    constexpr auto operator | (compose_fn<Ts...> l, R && r)
    {
        return compose(std::forward<R>(r), std::move(l));
    }
} // namespace burst

#endif // BURST_FUNCTIONAL_COMPOSE_HPP
