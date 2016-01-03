#ifndef BURST_RANGE_MAKE_RANGE_VECTOR_HPP
#define BURST_RANGE_MAKE_RANGE_VECTOR_HPP

#include <burst/container/make_vector.hpp>

#include <boost/range/iterator_range.hpp>

namespace burst
{
    //!     Превратить набор коллекций в std::vector диапазонов.
    /*!
            Принимает произвольное множество коллекций одного типа, создаёт для них диапазоны и
        возвращает вектор этих диапазонов в соответствующем порядке.
     */
    template <typename ... Collections>
    auto make_range_vector (Collections & ... collections)
    {
        return make_vector({boost::make_iterator_range(collections)...});
    }
} // namespace burst

#endif // BURST_RANGE_MAKE_RANGE_VECTOR_HPP
