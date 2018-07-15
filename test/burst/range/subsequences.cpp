#include <burst/range/subsequences.hpp>
#include <utility/io/vector.hpp>

#include <boost/test/unit_test.hpp>

#include <vector>

BOOST_AUTO_TEST_SUITE(subsequences)
    BOOST_AUTO_TEST_CASE(the_order_of_the_elements_does_not_matter)
    {
        const auto sequence = {3, 4, 1, -5};

        const auto subsequences = burst::subsequences(sequence);

        const auto expected_subsequences =
            std::vector<std::vector<int>>
            {
                {3}, {4}, {1}, {-5},
                {3, 4}, {3, 1}, {3, -5}, {4, 1}, {4, -5}, {1, -5},
                {3, 4, 1}, {3, 4, -5}, {3, 1, -5}, {4, 1, -5},
                {3, 4, 1, -5}
            };
        BOOST_CHECK_EQUAL(subsequences, expected_subsequences);
    }
BOOST_AUTO_TEST_SUITE_END()
