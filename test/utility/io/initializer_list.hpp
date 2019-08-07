#ifndef BURST_TEST_OUTPUT_INITIALIZER_LIST_HPP
#define BURST_TEST_OUTPUT_INITIALIZER_LIST_HPP

#include <utility/io/range.hpp>

#include <initializer_list>
#include <ostream>

namespace std
{
    template <typename T>
    std::ostream & operator << (std::ostream & stream, const std::initializer_list<T> & l)
    {
        return ::test_detail::print_range(stream, l);
    }
}

#endif // BURST_TEST_OUTPUT_INITIALIZER_LIST_HPP
