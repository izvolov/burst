#ifndef BURST_TEST_OUTPUT_RANGE_HPP
#define BURST_TEST_OUTPUT_RANGE_HPP

#include <burst/functional/identity.hpp>
#include <burst/type_traits/range_value.hpp>

#include <algorithm>
#include <iterator>
#include <ostream>
#include <type_traits>

namespace test_detail
{
    template <typename Range, typename UnaryFunction>
    std::ostream & print_range (std::ostream & stream, const Range & range, UnaryFunction proj)
    {
        stream << "[";

        using std::begin;
        using std::end;

        if (begin(range) != end(range))
        {
            stream << proj(*begin(range));
            std::for_each(std::next(begin(range)), end(range),
                [& stream, & proj] (const auto & value)
                {
                    stream << ", " << proj(value);
                });
        }

        return stream << "]";
    }

    template <typename Range>
    auto print_range (std::ostream & stream, const Range & range)
        ->
            typename std::enable_if
            <
                std::is_integral<burst::range_value_t<Range>>::value &&
                sizeof(burst::range_value_t<Range>) == 1,
                std::ostream &
            >
            ::type
    {
        return print_range(stream, range, [] (auto x) -> int {return x;});
    }

    template <typename Range>
    auto print_range (std::ostream & stream, const Range & range)
        ->
            typename std::enable_if
            <
                sizeof(burst::range_value_t<Range>) != 1,
                std::ostream &
            >
            ::type
    {
        return print_range(stream, range, burst::identity);
    }
}

#endif // BURST_TEST_OUTPUT_RANGE_HPP
