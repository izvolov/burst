#ifndef BURST__RANGE__BINARY_ISTREAM_RANGE_HPP
#define BURST__RANGE__BINARY_ISTREAM_RANGE_HPP

#include <burst/iterator/binary_istream_iterator.hpp>
#include <burst/iterator/end_tag.hpp>

#include <boost/range/iterator_range.hpp>

#include <istream>
#include <utility>

namespace burst
{
    template <typename Value, typename Read = trivial_read_fn>
    auto make_binary_istream_range (std::istream & stream, Read read = Read{})
    {
        auto begin = make_binary_istream_iterator<Value>(stream, read);
        auto end = make_binary_istream_iterator(iterator::end_tag, begin);

        return boost::make_iterator_range(std::move(begin), std::move(end));
    }
} // namespace burst

#endif // BURST__RANGE__BINARY_ISTREAM_RANGE_HPP
