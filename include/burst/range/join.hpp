#ifndef BURST_RANGE_JOIN_HPP
#define BURST_RANGE_JOIN_HPP

#include <burst/iterator/end_tag.hpp>
#include <burst/iterator/join_iterator.hpp>

#include <boost/range/iterator_range.hpp>

#include <utility>

namespace burst
{
    struct join_t
    {
        //!     Функция для создания склеенного диапазона.
        /*!
                Принимает на вход диапазон диапазонов, которые нужно склеить.
                Возвращает диапазон, представляющий из себя один большой диапазон, в котором
            входные диапазоны поставлены друг за другом таким образом, что после последнего
            элемента i-го диапазона сразу следует первый элемент (i + 1)-го.
         */
        template <typename Range>
        auto operator () (Range && ranges) const
        {
            auto begin = make_join_iterator(std::forward<Range>(ranges));
            auto end = make_join_iterator(iterator::end_tag, begin);

            return boost::make_iterator_range(std::move(begin), std::move(end));
        }
    };

    constexpr auto join = join_t{};
}

#endif // BURST_RANGE_JOIN_HPP
