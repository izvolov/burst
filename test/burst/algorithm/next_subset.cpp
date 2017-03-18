#include <burst/algorithm/next_subset.hpp>
#include <burst/container/make_vector.hpp>

#include <boost/iterator/indirect_iterator.hpp>
#include <boost/test/unit_test.hpp>

#include <functional>
#include <vector>

BOOST_AUTO_TEST_SUITE(next_subset)
    BOOST_AUTO_TEST_CASE(accepts_iterators_of_a_set_and_iterators_of_a_valid_subset)
    {
        using vector_type = std::vector<int>;
        auto items = vector_type{17};
        std::vector<vector_type::iterator> subset_container(items.size());

        auto new_subset_end =
            burst::next_subset
            (
                subset_container.begin(),
                subset_container.begin(),
                items.begin(),
                items.end()
            );

        BOOST_CHECK(new_subset_end == subset_container.begin() + 1);
        BOOST_CHECK_EQUAL(**subset_container.begin(), 17);
    }

    BOOST_AUTO_TEST_CASE(empty_set_has_no_subsets)
    {
        using vector_type = std::vector<int>;
        auto items = vector_type{};
        std::vector<vector_type::iterator> subset_container(10);

        auto new_subset_end =
            burst::next_subset
            (
                subset_container.begin(),
                subset_container.begin(),
                items.begin(),
                items.end()
            );

        BOOST_CHECK(new_subset_end == subset_container.begin());
    }

    BOOST_AUTO_TEST_CASE(allows_custom_ordering)
    {
        using vector_type = std::vector<int>;
        auto items = vector_type{3, 2, 1};
        std::vector<vector_type::iterator> subset_container(items.size());

        burst::next_subset
        (
            subset_container.begin(), subset_container.begin(),
            items.begin(), items.end(),
            std::greater<>{}
        );

        BOOST_CHECK_EQUAL(**subset_container.begin(), 3);
    }

    BOOST_AUTO_TEST_CASE(subsets_are_listed_in_a_natural_order)
    {
        using vector_type = std::vector<int>;
        auto items = vector_type{1, 2, 3};
        std::vector<vector_type::iterator> subset_container(items.size());

        auto previous_subset = std::vector<int>{};
        auto current_subset_end =
            burst::next_subset
            (
                subset_container.begin(), subset_container.begin(), items.begin(), items.end()
            );
        while (current_subset_end != subset_container.begin())
        {
            auto current_subset =
                burst::make_vector
                (
                    boost::make_indirect_iterator(subset_container.begin()),
                    boost::make_indirect_iterator(current_subset_end)
                );
            BOOST_CHECK(previous_subset.size() < current_subset.size() || previous_subset < current_subset);
            current_subset_end =
                burst::next_subset
                (
                    subset_container.begin(), current_subset_end, items.begin(), items.end()
                );
            previous_subset = current_subset;
        }
    }
BOOST_AUTO_TEST_SUITE_END()
