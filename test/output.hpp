#ifndef BURST_TEST_OUTPUT_HPP
#define BURST_TEST_OUTPUT_HPP

#include <algorithm>
#include <forward_list>
#include <iostream>
#include <iterator>
#include <utility>
#include <vector>

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

namespace std
{
    template <typename First, typename Second>
    std::ostream & operator << (std::ostream & stream, const std::pair<First, Second> & pair)
    {
        return stream << "{" << pair.first << ", " << pair.second << "}";
    }

    template <typename T>
    std::ostream & operator << (std::ostream & stream, const std::vector<T> & vector)
    {
        return ::test_detail::print_range(stream, vector);
    }

    template <typename T>
    std::ostream & operator << (std::ostream & stream, const std::forward_list<T> & list)
    {
        return ::test_detail::print_range(stream, list);
    }
}

#endif // BURST_TEST_OUTPUT_HPP
