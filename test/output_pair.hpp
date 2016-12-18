#ifndef BURST_TEST_OUTPUT_PAIR_HPP
#define BURST_TEST_OUTPUT_PAIR_HPP

#include <ostream>
#include <utility>

namespace std
{
    template <typename First, typename Second>
    std::ostream & operator << (std::ostream & stream, const std::pair<First, Second> & pair)
    {
        return stream << "{" << pair.first << ", " << pair.second << "}";
    }
}

#endif // BURST_TEST_OUTPUT_PAIR_HPP
