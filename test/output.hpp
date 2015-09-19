#ifndef BURST_TEST_OUTPUT_HPP
#define BURST_TEST_OUTPUT_HPP

#include <algorithm>
#include <iostream>
#include <iterator>
#include <utility>
#include <vector>

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
        stream << "[";

        if (not vector.empty())
        {
            stream << vector.front();
            std::for_each(std::next(vector.begin()), vector.end(),
                [& stream] (const auto & value)
                {
                    stream << ", " << value;
                });
        }

        return stream << "]";
    }
}

#endif // BURST_TEST_OUTPUT_HPP
