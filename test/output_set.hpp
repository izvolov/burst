#ifndef BURST_TEST_OUTPUT_SET_HPP
#define BURST_TEST_OUTPUT_SET_HPP

#include <test/output_range.hpp>

#include <ostream>
#include <set>

namespace std
{
    template <typename ... Ts>
    std::ostream & operator << (std::ostream & stream, const std::set<Ts...> & set)
    {
        return ::test_detail::print_range(stream, set);
    }
}

#endif // BURST_TEST_OUTPUT_SET_HPP
