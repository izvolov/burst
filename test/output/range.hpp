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

        if (not range.empty())
        {
            stream << *std::begin(range);
            std::for_each(std::next(range.begin()), range.end(),
                [& stream] (const auto & value)
                {
                    stream << ", " << value;
                });
        }

        return stream << "]";
    }
}

#endif // BURST_TEST_OUTPUT_RANGE_HPP
