#ifndef BURST_RANGE_CACHE_ONE_HPP
#define BURST_RANGE_CACHE_ONE_HPP

#include <iterator>

#include <boost/range/iterator_range.hpp>

#include <burst/iterator/cache_iterator.hpp>

namespace burst
{
    //!     Функция для создания диапазона кэширующих итераторов
    /*!
            Принимает на вход произвольный диапазон.
            Возвращает диапазон из кэширующих итераторов.
     */
    template <typename Range>
    auto cache_one (const Range & range)
    {
        return
            boost::make_iterator_range
            (
                make_cache_iterator(std::begin(range)),
                make_cache_iterator(std::end(range))
            );
    }
} // namespace burst

#endif // BURST_RANGE_CACHE_ONE_HPP
