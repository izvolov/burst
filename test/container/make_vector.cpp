#include <burst/container/make_vector.hpp>
#include <output.hpp>

#include <boost/algorithm/cxx11/all_of.hpp>
#include <boost/range/irange.hpp>
#include <boost/test/unit_test.hpp>

#include <string>
#include <type_traits>
#include <vector>

BOOST_AUTO_TEST_SUITE(make_vector)
    BOOST_AUTO_TEST_CASE(value_type_of_created_vector_is_taken_from_value_type_of_incoming_range)
    {
        const auto range = boost::irange(0, 4);
        const auto v = burst::make_vector(range);
        BOOST_CHECK
        ((
            std::is_same
            <
                decltype(v)::value_type,
                boost::range_value<decltype(range)>::type
            >
            ::value
        ));
    }

    BOOST_AUTO_TEST_CASE(accepts_range_by_rvalue)
    {
        const auto v = burst::make_vector(boost::irange<std::uint32_t>(0, 6));
        BOOST_CHECK_EQUAL(v, (std::vector<std::uint32_t>{0, 1, 2, 3, 4, 5}));
    }

    BOOST_AUTO_TEST_CASE(accepts_range_by_lvalue)
    {
        std::stringstream stream("5 4 3 2");
        const auto range =
            boost::make_iterator_range
            (
                std::istream_iterator<int>(stream),
                std::istream_iterator<int>{}
            );

        const auto v = burst::make_vector(range);
        BOOST_CHECK_EQUAL(v, (std::vector<int>{5, 4, 3, 2}));
    }

    BOOST_AUTO_TEST_CASE(overloaded_for_initializer_list)
    {
        const auto v = burst::make_vector({1, 2, 3});
        BOOST_CHECK_EQUAL(v, (std::vector<int>{1, 2, 3}));
    }

    BOOST_AUTO_TEST_CASE(overloaded_for_initializer_list_with_allocator)
    {
        const auto v = burst::make_vector({4, 5, 6}, std::allocator<int>{});
        BOOST_CHECK_EQUAL(v, (std::vector<int>{4, 5, 6}));
    }

    BOOST_AUTO_TEST_CASE(overloaded_for_range_with_allocator)
    {
        const auto v =
            burst::make_vector
            (
                boost::irange<std::uint32_t>(0, 6),
                std::allocator<std::uint32_t>{}
            );

        BOOST_CHECK_EQUAL(v, (std::vector<std::uint32_t>{0, 1, 2, 3, 4, 5}));
    }

    BOOST_AUTO_TEST_CASE(value_type_may_be_specified_explicitly_when_constructed_from_range)
    {
        const auto v = burst::make_vector<std::size_t>(boost::irange<int>(0, 4));
        BOOST_CHECK
        ((
            std::is_same
            <
                decltype(v)::value_type,
                std::size_t
            >
            ::value
        ));
    }

    BOOST_AUTO_TEST_CASE(value_type_may_be_specified_explicitly_when_constructed_from_range_with_allocator)
    {
        const auto v =
            burst::make_vector<std::size_t>
            (
                boost::irange<int>(0, 4),
                std::allocator<std::size_t>{}
            );
        BOOST_CHECK
        ((
            std::is_same
            <
                decltype(v)::value_type,
                std::size_t
            >
            ::value
        ));
    }

    BOOST_AUTO_TEST_CASE(constructs_with_n_copies_of_a_given_value)
    {
        const auto v = burst::make_vector(5u, std::string{});

        BOOST_CHECK_EQUAL(v.size(), 5);
        BOOST_CHECK((std::is_same<decltype(v)::value_type, std::string>::value));
        BOOST_CHECK(boost::algorithm::all_of(v, [] (const auto & s) {return s.empty();}));
    }

    BOOST_AUTO_TEST_CASE(overloaded_for_two_iterators)
    {
        const auto l = {1, 2, 3};
        const auto v = burst::make_vector(l.begin(), l.end());
        BOOST_CHECK_EQUAL(v, (std::vector<int>{1, 2, 3}));
    }

    BOOST_AUTO_TEST_CASE(overloaded_for_two_iterators_with_explicit_value_type)
    {
        const auto l = {1, 2, 3};
        const auto v = burst::make_vector<short>(l.begin(), l.end());
        BOOST_CHECK
        ((
            std::is_same
            <
                decltype(v)::value_type,
                short
            >
            ::value
        ));
    }

    BOOST_AUTO_TEST_CASE(overloaded_for_two_iterators_with_allocator)
    {
        const auto l = {1, 2, 3};
        const auto v = burst::make_vector(l.begin(), l.end(), std::allocator<int>{});
        BOOST_CHECK_EQUAL(v, (std::vector<int>{1, 2, 3}));
    }

    BOOST_AUTO_TEST_CASE(overloaded_for_two_iterators_with_allocator_and_explicit_value_type)
    {
        const auto l = {1, 2, 3};
        const auto v = burst::make_vector<double>(l.begin(), l.end(), std::allocator<double>{});
        BOOST_CHECK
        ((
            std::is_same
            <
                decltype(v)::value_type,
                double
            >
            ::value
        ));
    }
BOOST_AUTO_TEST_SUITE_END()
