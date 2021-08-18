#ifndef BURST__RANGE__MERGE_HPP
#define BURST__RANGE__MERGE_HPP

#include <burst/iterator/end_tag.hpp>
#include <burst/iterator/merge_iterator.hpp>

#include <boost/range/iterator_range.hpp>

#include <utility>

namespace burst
{
    struct merge_t
    {
        /*!
            \brief
                Функция для создания диапазона слияния

            \details
                Создаёт итератор слияния посредством пробрасывания аргументов этой функции в
                функцию `make_merge_iterator`, а из этого итератора создаёт диапазон.

            \returns
                Диапазон, представляющий собой слияние нескольких (переданных в аргументах)
                диапазонов.

            \see make_merge_iterator
            \see merge_iterator
         */
        template <typename ... Args>
        auto operator () (Args && ... args) const
        {
            auto begin = make_merge_iterator(std::forward<Args>(args)...);
            auto end = make_merge_iterator(iterator::end_tag, begin);

            return boost::make_iterator_range(std::move(begin), std::move(end));
        }
    };

    constexpr auto merge = merge_t{};
}

#endif // BURST__RANGE__MERGE_HPP
