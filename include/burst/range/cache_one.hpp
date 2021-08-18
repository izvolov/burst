#ifndef BURST__RANGE__CACHE_ONE_HPP
#define BURST__RANGE__CACHE_ONE_HPP

#include <burst/iterator/cache_iterator.hpp>

#include <boost/range/iterator_range.hpp>

#include <iterator>

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
        using std::begin;
        using std::end;
        return
            boost::make_iterator_range
            (
                make_cache_iterator(begin(range)),
                make_cache_iterator(end(range))
            );
    }
} // namespace burst

#endif // BURST__RANGE__CACHE_ONE_HPP
