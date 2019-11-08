#include <burst/range/adaptor/cached_one.hpp>

#include <doctest/doctest.h>

#include <boost/range/irange.hpp>

TEST_SUITE("cached_one")
{
    TEST_CASE("Может работать с lvalue-диапазоном")
    {
        const auto range = boost::irange(0, 5);

        const auto cached = range | burst::cached_one;

        CHECK(cached == range);
    }

    TEST_CASE("Может работать с rvalue-диапазоном")
    {
        const auto cached = boost::irange(0, 5) | burst::cached_one;
        CHECK(cached == boost::irange(0, 5));
    }
}
