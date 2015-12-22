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
        return
            boost::make_iterator_range
            (
                make_binary_istream_iterator<Value>(stream, read),
                make_binary_istream_iterator<Value, Read>(iterator::end_tag, read)
            );
    }
} // namespace burst

#endif // BURST_ITERATOR_BINARY_ISTREAM_RANGE_HPP
