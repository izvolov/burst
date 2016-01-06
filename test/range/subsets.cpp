#include <burst/range/subsets.hpp>

#include <boost/test/unit_test.hpp>

#include <functional>
#include <set>

BOOST_AUTO_TEST_SUITE(subsets)
    BOOST_AUTO_TEST_CASE(repeating_elements_are_not_considered)
    {
        auto sequence = {3, 3, 3, 2, 2, 2, 1, 1, 1};
        std::set<int, std::greater<int>> set(sequence.begin(), sequence.end());

        auto sequence_subsets =
            burst::subsets
            (
                boost::make_iterator_range(sequence),
                std::greater<int>()
            );

        auto set_subsets =
            burst::subsets
            (
                boost::make_iterator_range(set),
                std::greater<int>()
            );

        BOOST_CHECK_EQUAL(sequence_subsets, set_subsets);
    }
BOOST_AUTO_TEST_SUITE_END()
