#ifndef BURST_TEST_OUTPUT_DEQUE_HPP
#define BURST_TEST_OUTPUT_DEQUE_HPP

#include <test/output_range.hpp>

#include <deque>
#include <ostream>

namespace std
{
    template <typename T>
    std::ostream & operator << (std::ostream & stream, const std::deque<T> & deque)
    {
        return ::test_detail::print_range(stream, deque);
    }
}

#endif // BURST_TEST_OUTPUT_DEQUE_HPP
