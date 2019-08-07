#include <burst/container/make_vector.hpp>
#include <utility/io/deque.hpp>
#include <utility/io/forward_list.hpp>
#include <utility/io/list.hpp>
#include <utility/io/vector.hpp>

#include <doctest/doctest.h>

#include <boost/algorithm/cxx11/all_of.hpp>
#include <boost/mpl/push_back.hpp>
#include <boost/mpl/vector.hpp>
#include <boost/range/irange.hpp>

#include <deque>
#include <forward_list>
#include <initializer_list>
#include <list>
#include <sstream>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

namespace
{
    template <template <typename ...> class SequenceContainer>
    struct make_sequence_t
    {
        template <typename Value, typename ... Xs>
        static auto apply (std::initializer_list<Value> values, Xs && ... xs)
        {
            return burst::make_sequence_container<SequenceContainer>(values, std::forward<Xs>(xs)...);
        }

        template <typename ... Xs>
        static auto apply (Xs && ... xs)
        {
            return burst::make_sequence_container<SequenceContainer>(std::forward<Xs>(xs)...);
        }

        template <typename Value, typename ... Xs>
        static auto apply (Xs && ... xs)
        {
            return burst::make_sequence_container<SequenceContainer, Value>(std::forward<Xs>(xs)...);
        }

        template <typename Value>
        static auto actual (std::initializer_list<Value> values)
        {
            return SequenceContainer<Value>(values);
        }
    };
}

#define SIZED_SEQUENCE_GENERATORS\
    make_sequence_t<std::deque>,\
    make_sequence_t<std::list>,\
    make_sequence_t<std::vector>

#define SEQUENCE_GENERATORS\
    SIZED_SEQUENCE_GENERATORS,\
    make_sequence_t<std::forward_list>

TEST_SUITE("make_sequence_container")
{
    TEST_CASE_TEMPLATE("value_type_of_created_sequence_is_taken_from_value_type_of_incoming_range",
        make_sequence, SEQUENCE_GENERATORS)
    {
        const auto range = boost::irange(0, 4);
        const auto v = make_sequence::apply(range);
        CHECK
        ((
            std::is_same
            <
                typename decltype(v)::value_type,
                boost::range_value<decltype(range)>::type
            >
            ::value
        ));
    }

    TEST_CASE_TEMPLATE("accepts_range_by_rvalue",
        make_sequence, SEQUENCE_GENERATORS)
    {
        const auto v = make_sequence::apply(boost::irange<std::uint32_t>(0, 6));
        CHECK(v == make_sequence::template actual<std::uint32_t>({0, 1, 2, 3, 4, 5}));
    }

    TEST_CASE_TEMPLATE("accepts_range_by_lvalue",
        make_sequence, SEQUENCE_GENERATORS)
    {
        std::stringstream stream("5 4 3 2");
        const auto range =
            boost::make_iterator_range
            (
                std::istream_iterator<int>(stream),
                std::istream_iterator<int>{}
            );

        const auto v = make_sequence::apply(range);
        CHECK(v == make_sequence::template actual<int>({5, 4, 3, 2}));
    }

    TEST_CASE_TEMPLATE("accepts_containers",
        make_sequence, SEQUENCE_GENERATORS)
    {
        const auto initial = std::vector<int>{1, 2, 3};

        const auto v = make_sequence::apply(initial);
        CHECK(v == make_sequence::template actual<int>({1, 2, 3}));
    }

    TEST_CASE_TEMPLATE("accepts_rvalue_containers",
        make_sequence, SEQUENCE_GENERATORS)
    {
        const auto v = make_sequence::apply(std::vector<int>{1, 2, 3});
        CHECK(v == make_sequence::template actual<int>({1, 2, 3}));
    }

    TEST_CASE_TEMPLATE("overloaded_for_initializer_list",
        make_sequence, SEQUENCE_GENERATORS)
    {
        const auto v = make_sequence::apply({1, 2, 3});
        CHECK(v == make_sequence::template actual<int>({1, 2, 3}));
    }

    TEST_CASE_TEMPLATE("overloaded_for_initializer_list_with_allocator",
        make_sequence, SEQUENCE_GENERATORS)
    {
        const auto v = make_sequence::apply({4, 5, 6}, std::allocator<int>{});
        CHECK(v == make_sequence::template actual<int>({4, 5, 6}));
    }

    TEST_CASE_TEMPLATE("overloaded_for_range_with_allocator",
        make_sequence, SEQUENCE_GENERATORS)
    {
        const auto v =
            make_sequence::apply
            (
                boost::irange<std::uint32_t>(0, 6),
                std::allocator<std::uint32_t>{}
            );

        CHECK(v == make_sequence::template actual<std::uint32_t>({0, 1, 2, 3, 4, 5}));
    }

    TEST_CASE_TEMPLATE("value_type_may_be_specified_explicitly_when_constructed_from_range",
        make_sequence, SEQUENCE_GENERATORS)
    {
        const auto v = make_sequence::template apply<std::size_t>(boost::irange<int>(0, 4));
        CHECK
        ((
            std::is_same
            <
                typename decltype(v)::value_type,
                std::size_t
            >
            ::value
        ));
    }

    TEST_CASE_TEMPLATE("value_type_may_be_specified_explicitly_when_constructed_from_range_with_allocator",
        make_sequence, SEQUENCE_GENERATORS)
    {
        const auto v =
            make_sequence::template apply<std::size_t>
            (
                boost::irange<int>(0, 4),
                std::allocator<std::size_t>{}
            );
        CHECK
        ((
            std::is_same
            <
                typename decltype(v)::value_type,
                std::size_t
            >
            ::value
        ));
    }

    TEST_CASE_TEMPLATE("constructs_with_n_copies_of_a_given_value",
        make_sequence, SIZED_SEQUENCE_GENERATORS)
    {
        const auto v = make_sequence::apply(5u, std::string{});

        CHECK(v.size() == 5);
        CHECK((std::is_same<typename decltype(v)::value_type, std::string>::value));
        CHECK(boost::algorithm::all_of(v, [] (const auto & s) {return s.empty();}));
    }

    TEST_CASE("constructs_forward_list_with_n_copies_of_a_given_value")
    {
        const auto v = burst::make_sequence_container<std::forward_list>(5u, std::string{});

        CHECK(std::distance(v.begin(), v.end()) == 5);
        CHECK((std::is_same<decltype(v)::value_type, std::string>::value));
        CHECK(boost::algorithm::all_of(v, [] (const auto & s) {return s.empty();}));
    }

    TEST_CASE_TEMPLATE("overloaded_for_two_iterators",
        make_sequence, SEQUENCE_GENERATORS)
    {
        const auto l = {1, 2, 3};
        const auto v = make_sequence::apply(l.begin(), l.end());
        CHECK(v == make_sequence::template actual<int>({1, 2, 3}));
    }

    TEST_CASE_TEMPLATE("overloaded_for_two_iterators_with_explicit_value_type",
        make_sequence, SEQUENCE_GENERATORS)
    {
        const auto l = {1, 2, 3};
        const auto v = make_sequence::template apply<short>(l.begin(), l.end());
        CHECK
        ((
            std::is_same
            <
                typename decltype(v)::value_type,
                short
            >
            ::value
        ));
    }

    TEST_CASE_TEMPLATE("overloaded_for_two_iterators_with_allocator",
        make_sequence, SEQUENCE_GENERATORS)
    {
        const auto l = {1, 2, 3};
        const auto v = make_sequence::apply(l.begin(), l.end(), std::allocator<int>{});
        CHECK(v == make_sequence::template actual<int>({1, 2, 3}));
    }

    TEST_CASE_TEMPLATE("overloaded_for_two_iterators_with_allocator_and_explicit_value_type",
        make_sequence, SEQUENCE_GENERATORS)
    {
        const auto l = {1, 2, 3};
        const auto v = make_sequence::template apply<double>(l.begin(), l.end(), std::allocator<double>{});
        CHECK
        ((
            std::is_same
            <
                typename decltype(v)::value_type,
                double
            >
            ::value
        ));
    }
}
