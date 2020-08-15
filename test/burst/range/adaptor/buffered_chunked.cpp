#include <burst/container/make_forward_list.hpp>
#include <burst/container/make_vector.hpp>
#include <burst/range/adaptor/buffered_chunked.hpp>

#include <doctest/doctest.h>

#include <boost/range/iterator_range.hpp>

TEST_SUITE("buffered_chunked")
{
    TEST_CASE("Может работать с rvalue-диапазоном")
    {
        const auto v = burst::make_vector({1, 2, 3, 4, 5, 6});

        const auto chunks = boost::make_iterator_range(v) | burst::buffered_chunked(3);

        const auto expected = {1, 2, 3};
        CHECK(chunks.front() == expected);
    }

    TEST_CASE("Может работать с lvalue-диапазоном")
    {
        const auto v = burst::make_forward_list({3u, 2u, 1u});

        const auto chunks = v | burst::buffered_chunked(3);

        const auto expected = {3, 2, 1};
        CHECK(chunks.front() == expected);
    }
}
