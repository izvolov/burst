#ifndef BURST__RANGE__SEMIINTERSECT_HPP
#define BURST__RANGE__SEMIINTERSECT_HPP

#include <burst/iterator/end_tag.hpp>
#include <burst/iterator/semiintersect_iterator.hpp>

#include <boost/range/iterator_range.hpp>

#include <cstddef>
#include <utility>

namespace burst
{
    struct semiintersect_t
    {
        /*!
            \brief
                Функция для создания диапазона полупересечения

            \details
                Создаёт итератор полупересечения посредством пробрасывания аргументов этой функции в
                функцию `make_semiintersect_iterator`, а из этого итератора создаёт диапазон.

                `M`-полупересечение диапазонов — это такой набор элементов, которые есть
                одновременно хотя бы в `M` из `N` входных диапазонах.

            \returns
                Диапазон, представляющий собой полупересечение нескольких (переданных в аргументах)
                диапазонов.

            \see make_semiintersect_iterator
            \see semiintersect_iterator
         */
        template <typename ... Args>
        auto operator () (Args && ... args) const
        {
            auto begin = make_semiintersect_iterator(std::forward<Args>(args)...);
            auto end = make_semiintersect_iterator(iterator::end_tag, begin);

            return boost::make_iterator_range(std::move(begin), std::move(end));
        }
    };

    constexpr auto semiintersect = semiintersect_t{};
}

#endif // BURST__RANGE__SEMIINTERSECT_HPP
