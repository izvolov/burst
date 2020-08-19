#include <burst/container/make_forward_list.hpp>
#include <burst/container/make_vector.hpp>
#include <burst/range/take_at_most.hpp>
#include <utility/io/forward_list.hpp>

#include <doctest/doctest.h>

#include <boost/range/concepts.hpp>
#include <boost/range/iterator_range.hpp>

#include <iterator>

TEST_SUITE("take_at_most")
{
    TEST_CASE("Откусывает заданное количество элементов")
    {
        const auto items = burst::make_forward_list({1, 2, 3, 4});

        const auto items_to_take = 3;
        const auto taken = burst::take_at_most(items, items_to_take);

        const auto expected =
            boost::make_iterator_range(items.begin(), std::next(items.begin(), items_to_take));
        CHECK(taken == expected);
    }

    TEST_CASE("Не откусывает больше, чем есть...")
    {
        SUBCASE("... от итератора произвольного доступа")
        {
            const auto items = burst::make_vector({'a', 'b', 'c', 'd'});

            const auto taken = burst::take_at_most(items, items.size() * 100500);

            CHECK(std::distance(taken.begin(), taken.end()) == items.size());
        }
        SUBCASE("... от любого другого итератора")
        {
            const auto items = burst::make_forward_list({'a', 'b', 'c', 'd'});
            const auto size = std::distance(items.begin(), items.end());

            const auto taken = burst::take_at_most(items, size * 100500);

            CHECK(std::distance(taken.begin(), taken.end()) == size);
        }
    }

    TEST_CASE("Результирующий диапазон ссылается на исходные диапазоны, так что изменение "
        "элементов результирующего диапазона влечёт изменение соответствующих элементов исходного")
    {
        auto items = burst::make_forward_list({1, 2, 3, 4});

        const auto items_to_take = 3;
        auto taken = burst::take_at_most(items, items_to_take);

        for (auto & x: taken)
        {
            x *= 10;
        }

        CHECK(items == burst::make_forward_list({10, 20, 30, 4}));
    }

    TEST_CASE("Откусывание от диапазона произвольного доступа порождает диапазон произвольного "
        "доступа")
    {
        const int items[] = {1, 2, 3};

        const auto items_to_take = 3;
        const auto taken = burst::take_at_most(items, items_to_take);

        BOOST_CONCEPT_ASSERT((boost::RandomAccessIterator<decltype(std::begin(taken))>));
    }
}
