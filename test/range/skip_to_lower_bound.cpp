#include <burst/container/make_vector.hpp>
#include <burst/range/skip_to_lower_bound.hpp>

#include <boost/range/iterator_range.hpp>
#include <boost/test/unit_test.hpp>

#include <functional>
#include <vector>

BOOST_AUTO_TEST_SUITE(skip_to_lower_bound)
    BOOST_AUTO_TEST_CASE(skipping_to_value_in_empty_range_results_empty_range)
    {
        std::vector<int> empty_container;
        auto empty_range = boost::make_iterator_range(empty_container);

        burst::skip_to_lower_bound(empty_range, 5);

        BOOST_CHECK(empty_range.empty());
    }

    BOOST_AUTO_TEST_CASE(skipping_to_value_in_sorted_range_containing_that_value_makes_range_begin_at_first_occurence_of_the_value)
    {
        auto values = burst::make_vector({1, 5, 5, 7});
        auto sorted_range = boost::make_iterator_range(values);

        burst::skip_to_lower_bound(sorted_range, 5);

        BOOST_CHECK_EQUAL_COLLECTIONS
        (
            boost::begin(sorted_range), boost::end(sorted_range),
            boost::begin(values) + 1, boost::end(values)
        );
    }

    BOOST_AUTO_TEST_CASE(skipping_to_value_greater_than_all_values_in_range_makes_range_empty)
    {
        auto values = {1, 2, 3};
        auto sorted_range = boost::make_iterator_range(values);

        burst::skip_to_lower_bound(sorted_range, 100500);

        BOOST_CHECK(sorted_range.empty());
    }

    BOOST_AUTO_TEST_CASE(skippin_to_value_less_than_all_elements_in_range_does_not_modify_range)
    {
        auto values = {3, 2, 1};
        auto sorted_range = boost::make_iterator_range(values);
        auto sorted_range_copy = sorted_range;

        burst::skip_to_lower_bound(sorted_range, 100500, std::greater<int>());

        BOOST_CHECK_EQUAL_COLLECTIONS
        (
            boost::begin(sorted_range), boost::end(sorted_range),
            boost::begin(sorted_range_copy), boost::end(sorted_range_copy)
        );
    }

    BOOST_AUTO_TEST_CASE(skipping_to_value_absent_in_sorted_range_makes_range_begin_at_first_value_greater_than_desired)
    {
        std::vector<double> values{3.14, 2.71, 1.6};
        auto sorted_range = boost::make_iterator_range(values);

        burst::skip_to_lower_bound(sorted_range, 2.5, std::greater<double>());

        BOOST_CHECK_EQUAL_COLLECTIONS
        (
            boost::begin(sorted_range), boost::end(sorted_range),
            boost::begin(values) + 2, boost::end(values)
        );
    }
BOOST_AUTO_TEST_SUITE_END()
