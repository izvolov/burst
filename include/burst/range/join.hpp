#ifndef BURST__RANGE__JOIN_HPP
#define BURST__RANGE__JOIN_HPP

#include <burst/iterator/end_tag.hpp>
#include <burst/iterator/join_iterator.hpp>

#include <boost/range/iterator_range.hpp>

#include <utility>

namespace burst
{
    struct join_t
    {
        /*!
            \brief
                Функция для создания склеенного диапазона

            \details
                Склеивает несколько диапазонов в один логически единый диапазон.
                Для этого пробрасывает аргументы этой функции в функцию `make_join_iterator`, а из
                получившегося итератора создаёт диапазон.

            \returns
                Диапазон, представляющий из себя один большой диапазон, в котором входные диапазоны
                поставлены друг за другом таким образом, что после последнего элемента `i`-го
                диапазона сразу следует первый элемент `(i + 1)`-го.

            \see make_join_iterator
            \see join_iterator
         */
        template <typename ... Args>
        auto operator () (Args && ... args) const
        {
            auto begin = make_join_iterator(std::forward<Args>(args)...);
            auto end = make_join_iterator(iterator::end_tag, begin);

            return boost::make_iterator_range(std::move(begin), std::move(end));
        }
    };

    constexpr auto join = join_t{};
}

#endif // BURST__RANGE__JOIN_HPP
