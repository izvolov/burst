#ifndef BURST_TEST_OUTPUT_FORWARD_LIST_HPP
#define BURST_TEST_OUTPUT_FORWARD_LIST_HPP

#include <utility/io/range.hpp>

#include <forward_list>
#include <ostream>

namespace std
{
    template <typename T>
    std::ostream & operator << (std::ostream & stream, const std::forward_list<T> & list)
    {
        return ::test_detail::print_range(stream, list);
    }
}

#endif // BURST_TEST_OUTPUT_FORWARD_LIST_HPP
