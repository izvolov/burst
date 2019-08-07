#include <burst/range/adaptor/cached_one.hpp>

#include <doctest/doctest.h>

#include <boost/range/irange.hpp>

TEST_SUITE("cached_one")
{
    TEST_CASE("accepts_a_range_by_lvalue")
    {
        const auto range = boost::irange(0, 5);

        const auto cached = range | burst::cached_one;

        CHECK(cached == range);
    }

    TEST_CASE("accepts_a_range_by_rvalue")
    {
        const auto cached = boost::irange(0, 5) | burst::cached_one;
        CHECK(cached == boost::irange(0, 5));
    }
}
