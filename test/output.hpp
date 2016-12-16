#ifndef BURST_TEST_OUTPUT_HPP
#define BURST_TEST_OUTPUT_HPP

#include <algorithm>
#include <deque>
#include <forward_list>
#include <iostream>
#include <iterator>
#include <list>
#include <set>
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

    template <typename Tuple, std::size_t Head, std::size_t ... Tail>
    std::ostream & print_tuple_impl (std::ostream & stream, const Tuple & t, std::index_sequence<Head, Tail...>)
    {
        stream << "(";
        stream << std::get<Head>(t);
        auto x = {(stream << ", " << std::get<Tail>(t), 0)...};
        static_cast<void>(x);
        stream << ")";

        return stream;
    }

    template <typename Tuple>
    std::ostream & print_tuple_impl (std::ostream & stream, const Tuple &, std::index_sequence<>)
    {
        return stream << "()";
    }

    template <typename ... Arguments>
    std::ostream & print_tuple (std::ostream & stream, const std::tuple<Arguments...> & t)
    {
        return print_tuple_impl(stream, t, std::index_sequence_for<Arguments...>{});
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
    std::ostream & operator << (std::ostream & stream, const std::list<T> & list)
    {
        return ::test_detail::print_range(stream, list);
    }

    template <typename T>
    std::ostream & operator << (std::ostream & stream, const std::deque<T> & deque)
    {
        return ::test_detail::print_range(stream, deque);
    }

    template <typename T>
    std::ostream & operator << (std::ostream & stream, const std::forward_list<T> & list)
    {
        return ::test_detail::print_range(stream, list);
    }

    template <typename ... Ts>
    std::ostream & operator << (std::ostream & stream, const std::set<Ts...> & set)
    {
        return ::test_detail::print_range(stream, set);
    }

    template <typename ... Arguments>
    std::ostream & operator << (std::ostream & stream, const std::tuple<Arguments...> & t)
    {
        return ::test_detail::print_tuple(stream, t);
    }
}

#endif // BURST_TEST_OUTPUT_HPP
