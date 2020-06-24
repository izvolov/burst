#ifndef BURST_RANGE_INTERSECT_HPP
#define BURST_RANGE_INTERSECT_HPP

#include <burst/iterator/end_tag.hpp>
#include <burst/iterator/intersect_iterator.hpp>

#include <boost/range/iterator_range.hpp>

#include <utility>

namespace burst
{
    struct intersect_t
    {
        /*!
            \brief
                Функция для создания диапазона пересечения

            \details
                Создаёт итератор пересечения посредством пробрасывания аргументов этой функции в
                функцию `make_intersect_iterator`, а из этого итератора создаёт диапазон.

                Пересечение диапазонов рассматривается в смысле пересечения множеств, то есть в
                результирующем диапазоне будут те, и только те элементы, которые есть одновременно
                во всех входных диапазонах.

            \returns
                Диапазон, представляющий собой пересечение нескольких (переданных в аргументах)
                диапазонов.

            \see make_intersect_iterator
            \see intersect_iterator
         */
        template <typename ... Args>
        auto operator () (Args && ... args) const
        {
            auto begin = make_intersect_iterator(std::forward<Args>(args)...);
            auto end = make_intersect_iterator(iterator::end_tag, begin);

            return boost::make_iterator_range(std::move(begin), std::move(end));
        }
    };

    constexpr auto intersect = intersect_t{};
}

#endif // BURST_RANGE_INTERSECT_HPP
