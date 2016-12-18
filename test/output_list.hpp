#ifndef BURST_TEST_OUTPUT_LIST_HPP
#define BURST_TEST_OUTPUT_LIST_HPP

#include <test/output_range.hpp>

#include <ostream>
#include <list>

namespace std
{
    template <typename T>
    std::ostream & operator << (std::ostream & stream, const std::list<T> & list)
    {
        return ::test_detail::print_range(stream, list);
    }
}

#endif // BURST_TEST_OUTPUT_LIST_HPP
