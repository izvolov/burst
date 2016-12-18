#include <burst/container/make_forward_list.hpp>
#include <burst/container/make_vector.hpp>
#include <burst/range/take_n.hpp>
#include <test/output_forward_list.hpp>

#include <boost/range/concepts.hpp>
#include <boost/test/unit_test.hpp>

#include <iterator>

BOOST_AUTO_TEST_SUITE(take_n)
    BOOST_AUTO_TEST_CASE(takes_exactly_specified_number_of_elements)
    {
        const auto items = burst::make_forward_list({1, 2, 3, 4});

        const auto items_to_take = 3;
        const auto taken = burst::take_n(items, items_to_take);

        BOOST_CHECK_EQUAL_COLLECTIONS
        (
            std::begin(items), std::next(std::begin(items), items_to_take),
            std::begin(taken), std::end(taken)
        );
    }

    BOOST_AUTO_TEST_CASE(pokerface_taking_too_many_elements)
    {
        const auto items = burst::make_vector({'a', 'b', 'c', 'd'});

        const auto items_to_take = 100500;
        const auto taken = burst::take_n(items, items_to_take);

        BOOST_CHECK_EQUAL(std::distance(taken.begin(), taken.end()), items_to_take);
    }

    BOOST_AUTO_TEST_CASE(taken_elements_of_mutable_range_can_be_modified)
    {
        auto items = burst::make_forward_list({1, 2, 3, 4});

        const auto items_to_take = 3;
        auto taken = burst::take_n(items, items_to_take);

        for (auto & x: taken)
        {
            x *= 10;
        }

        BOOST_CHECK_EQUAL(items, burst::make_forward_list({10, 20, 30, 4}));
    }

    BOOST_AUTO_TEST_CASE(taking_random_access_range_produces_random_access_range)
    {
        const int items[] = {1, 2, 3};

        const auto items_to_take = 3;
        const auto taken = burst::take_n(items, items_to_take);

        BOOST_CONCEPT_ASSERT((boost::RandomAccessIterator<decltype(std::begin(taken))>));
    }
BOOST_AUTO_TEST_SUITE_END()
