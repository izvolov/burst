#ifndef BURST_TEST_OUTPUT_VECTOR_HPP
#define BURST_TEST_OUTPUT_VECTOR_HPP

#include <test/output_range.hpp>

#include <ostream>
#include <vector>

namespace std
{
    template <typename T>
    std::ostream & operator << (std::ostream & stream, const std::vector<T> & vector)
    {
        return ::test_detail::print_range(stream, vector);
    }
}

#endif // BURST_TEST_OUTPUT_VECTOR_HPP
