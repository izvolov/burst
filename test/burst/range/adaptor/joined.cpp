#include <burst/container/make_forward_list.hpp>
#include <burst/range/adaptor/joined.hpp>
#include <burst/range/make_range_vector.hpp>
#include <utility/io/initializer_list.hpp>

#include <doctest/doctest.h>

#include <boost/range/adaptor/transformed.hpp>
#include <boost/range/irange.hpp>

TEST_SUITE("joined")
{
    TEST_CASE("Делает ровно то же самое, что функция burst::join")
    {
        const auto first = burst::make_forward_list({17, 19, 23});
        const auto empty1 = burst::make_forward_list<int>({});
        const auto second = burst::make_forward_list({29, 31, 37});
        const auto empty2 = burst::make_forward_list<int>({});

        auto ranges = burst::make_range_vector(first, empty1, second, empty2);

        const auto joined = ranges | burst::joined;

        const auto expected = {17, 19, 23, 29, 31, 37};
        CHECK(joined == expected);
    }

    TEST_CASE("Может работать с rvalue-диапазоном")
    {
        const auto joined =
            boost::irange(0, 10)
                | boost::adaptors::transformed([] (auto n) { return boost::irange(n * 2, n * 2 + 2); })
                | burst::joined;

        const auto expected = boost::irange(0, 20);
        CHECK(joined == expected);
    }
}
