#ifndef BURST_TEST_OUTPUT_RANGE_HPP
#define BURST_TEST_OUTPUT_RANGE_HPP

#include <algorithm>
#include <ostream>
#include <iterator>

namespace test_detail
{
    template <typename Range>
    std::ostream & print_range (std::ostream & stream, const Range & range)
    {
        stream << "[";

        using std::begin;
        using std::end;

        if (begin(range) != end(range))
        {
            stream << *begin(range);
            std::for_each(std::next(begin(range)), end(range),
                [& stream] (const auto & value)
                {
                    stream << ", " << value;
                });
        }

        return stream << "]";
    }
}

#endif // BURST_TEST_OUTPUT_RANGE_HPP
