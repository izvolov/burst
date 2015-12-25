#ifndef BURST_ITERATOR_BINARY_ISTREAM_RANGE_HPP
#define BURST_ITERATOR_BINARY_ISTREAM_RANGE_HPP

#include <istream>

#include <boost/range/iterator_range.hpp>

#include <burst/iterator/binary_istream_iterator.hpp>
#include <burst/iterator/end_tag.hpp>

namespace burst
{
    template <typename Value, typename Read = detail::trivial_read_t>
    auto make_binary_istream_range (std::istream & stream, Read read = Read{})
    {
        auto begin = make_binary_istream_iterator<Value>(stream, read);
        auto end = make_binary_istream_iterator(begin, iterator::end_tag);

        return boost::make_iterator_range(std::move(begin), std::move(end));
    }
} // namespace burst

#endif // BURST_ITERATOR_BINARY_ISTREAM_RANGE_HPP
