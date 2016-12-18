#ifndef BURST_TEST_OUTPUT_TUPLE_HPP
#define BURST_TEST_OUTPUT_TUPLE_HPP

#include <cstddef>
#include <ostream>
#include <tuple>
#include <utility>

namespace test_detail
{
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
    template <typename ... Arguments>
    std::ostream & operator << (std::ostream & stream, const std::tuple<Arguments...> & t)
    {
        return ::test_detail::print_tuple(stream, t);
    }
}

#endif // BURST_TEST_OUTPUT_TUPLE_HPP
