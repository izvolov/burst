#include <burst/container/make_forward_list.hpp>
#include <burst/range/adaptor/taken_at_most.hpp>
#include <burst/range/istream_range.hpp>

#include <doctest/doctest.h>

#include <boost/range/irange.hpp>
#include <boost/range/iterator_range.hpp>

#include <istream>
#include <iterator>
#include <sstream>

TEST_SUITE("taken_at_most")
{
    TEST_CASE("Создаёт диапазон из префикса исходного диапазона заданной длины")
    {
        const auto items = burst::make_forward_list({1, 2, 3, 4});

        const auto items_to_take = 3l;
        const auto taken = items | burst::taken_at_most(items_to_take);

        const auto expected =
            boost::make_iterator_range(items.begin(), std::next(items.begin(), items_to_take));

        CHECK(taken == expected);
    }

    TEST_CASE("Может работать с rvalue-диапазоном")
    {
        std::stringstream s("0 1 2 3 4");

        const auto taken = burst::make_istream_range<int>(s) | burst::taken_at_most(100500l);

        CHECK(taken == boost::irange(0, 5));
    }
}
