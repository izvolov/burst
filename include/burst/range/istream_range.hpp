#ifndef BURST_RANGE_ISTREAM_RANGE_HPP
#define BURST_RANGE_ISTREAM_RANGE_HPP

#include <boost/range/iterator_range.hpp>

#include <istream>
#include <iterator>

namespace burst
{
    /*!
        \brief
            Диапазон из std::istream_iterator
     */
    template <typename T>
    auto make_istream_range (std::istream & s)
    {
        return boost::make_iterator_range(std::istream_iterator<T>(s), std::istream_iterator<T>{});
    }
} // namespace burst

#endif // BURST_RANGE_ISTREAM_RANGE_HPP
