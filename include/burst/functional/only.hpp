#ifndef BURST__FUNCTIONAL__ONLY_HPP
#define BURST__FUNCTIONAL__ONLY_HPP

#include <burst/functional/tupless.hpp>
#include <burst/tuple/by.hpp>

#include <cstddef>
#include <utility>

namespace burst
{
    //!     Применить функцию к одному из входных аргументов
    /*!
            Порождает функциональный объект, который, будучи включённым в цепочку преобразователей,
        применит заданную функцию `f` к `N`-му из входных аргументов, а все остальные аргументы
        пробросит дальше без изменения.

            Пример:

            auto square = [] (auto x) {return x * x;};
            auto sum3 = [] (auto x, auto y, auto z) {return x + y + z;};

            auto f = only<1>(square) | sum3;

            auto r = f(2, 3, 4); // 2 + 3 * 3 + 4
     */
    template <std::size_t N, typename UnaryFunction>
    constexpr auto only (UnaryFunction && f)
    {
        return tupless(by<N>(std::forward<UnaryFunction>(f)));
    }
} // namespace burst

#endif // BURST__FUNCTIONAL__ONLY_HPP
