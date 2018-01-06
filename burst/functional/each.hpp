#ifndef BURST_FUNCTIONAL_EACH_HPP
#define BURST_FUNCTIONAL_EACH_HPP

#include <burst/functional/tupless.hpp>
#include <burst/tuple/by_all.hpp>

#include <utility>

namespace burst
{
    //!     Применить функцию к каждому из входных аргументов
    /*!
            Порождает функциональный объект, который, будучи включённым в цепочку преобразователей,
        применит заданную функцию `f` к каждому из входных аргументов.

            Пример:

            auto square = [] (auto x) {return x * x;};
            auto sum3 = [] (auto x, auto y, auto z) {return x + y + z;};

            auto f = each(square) | sum3;

            auto r = f(2, 3, 4); // 2 * 2 + 3 * 3 + 4 * 4
     */
    template <typename UnaryFunction>
    constexpr auto each (UnaryFunction && f)
    {
        return tupless(by_all(std::forward<UnaryFunction>(f)));
    }
} // namespace burst

#endif // BURST_FUNCTIONAL_EACH_HPP
