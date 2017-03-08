#include <burst/container/make_forward_list.hpp>
#include <burst/container/make_vector.hpp>
#include <burst/range/own_as_range.hpp>

#include <boost/test/unit_test.hpp>

#include <algorithm>
#include <iterator>

BOOST_AUTO_TEST_SUITE(own_as_range)
    BOOST_AUTO_TEST_CASE(iterates_over_the_owned_container)
    {
        const auto owned = burst::own_as_range(burst::make_vector({1, 2, 3, 4}));

        const auto expected = {1, 2, 3, 4};
        BOOST_CHECK_EQUAL_COLLECTIONS
        (
            std::begin(owned), std::end(owned),
            std::begin(expected), std::end(expected)
        );
    }

    BOOST_AUTO_TEST_CASE(owned_range_is_mutable)
    {
        auto owned = burst::own_as_range(burst::make_forward_list({1, 2, 3, 4}));
        std::transform(owned.begin(), owned.end(), owned.begin(), [] (auto x) {return x * 10;});

        const auto expected = {10, 20, 30, 40};
        BOOST_CHECK_EQUAL_COLLECTIONS
        (
            std::begin(owned), std::end(owned),
            std::begin(expected), std::end(expected)
        );
    }
BOOST_AUTO_TEST_SUITE_END()
