#include <burst/container/make_forward_list.hpp>
#include <burst/range/adaptor/owned_as_range.hpp>

#include <boost/test/unit_test.hpp>

#include <iterator>

BOOST_AUTO_TEST_SUITE(owned_as_range)
    BOOST_AUTO_TEST_CASE(acts_just_like_own_as_range_function)
    {
        const auto owned_as_range = burst::make_forward_list({'a', 'b', 'c'}) | burst::owned_as_range;

        const auto expected = {'a', 'b', 'c'};
        BOOST_CHECK_EQUAL_COLLECTIONS
        (
            std::begin(owned_as_range), std::end(owned_as_range),
            std::begin(expected), std::end(expected)
        );
    }
BOOST_AUTO_TEST_SUITE_END()
