#include <burst/container/make_forward_list.hpp>
#include <burst/range/adaptor/taken_n.hpp>

#include <boost/range/irange.hpp>
#include <boost/test/unit_test.hpp>

#include <iterator>

BOOST_AUTO_TEST_SUITE(taken_n)
    BOOST_AUTO_TEST_CASE(takes_exactly_specified_number_of_elements)
    {
        const auto items = burst::make_forward_list({1, 2, 3, 4});

        const auto items_to_take = 3l;
        const auto taken = items | burst::taken_n(items_to_take);

        BOOST_CHECK_EQUAL_COLLECTIONS
        (
            std::begin(items), std::next(std::begin(items), items_to_take),
            std::begin(taken), std::end(taken)
        );
    }

    BOOST_AUTO_TEST_CASE(accepts_a_range_by_rvalue)
    {
        const auto taken = boost::irange(0, 100500) | burst::taken_n(5l);

        BOOST_CHECK(taken == boost::irange(0, 5));
    }
BOOST_AUTO_TEST_SUITE_END()
