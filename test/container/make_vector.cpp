#include <burst/container/make_vector.hpp>
#include <output.hpp>

#include <boost/range/irange.hpp>
#include <boost/test/unit_test.hpp>

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
BOOST_AUTO_TEST_SUITE_END()
