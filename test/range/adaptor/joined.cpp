#include <iterator>

#include <boost/range/irange.hpp>
#include <boost/range/iterator_range.hpp>
#include <boost/test/unit_test.hpp>

#include <burst/container/make_forward_list.hpp>
#include <burst/range/adaptor/joined.hpp>
#include <burst/range/make_range_vector.hpp>

BOOST_AUTO_TEST_SUITE(joined)
    BOOST_AUTO_TEST_CASE(acts_just_like_join_function)
    {
        const auto first = burst::make_forward_list({17, 19, 23});
        const auto empty1 = burst::make_forward_list<int>({});
        const auto second = burst::make_forward_list({29, 31, 37});
        const auto empty2 = burst::make_forward_list<int>({});

        auto ranges = burst::make_range_vector(first, empty1, second, empty2);
        const auto to_join = boost::make_iterator_range(ranges);

        const auto joined = to_join | burst::joined;

        const auto expected = {17, 19, 23, 29, 31, 37};
        BOOST_CHECK_EQUAL_COLLECTIONS
        (
            std::begin(joined), std::end(joined),
            std::begin(expected), std::end(expected)
        );
    }

    BOOST_AUTO_TEST_CASE(accepts_a_range_by_rvalue)
    {
        const auto joined =
            boost::irange(0, 10)
                | boost::adaptors::transformed([] (auto n) { return boost::irange(n, n + 1); })
                | burst::joined;

        BOOST_CHECK(joined == boost::irange(0, 10));
    }
BOOST_AUTO_TEST_SUITE_END()
