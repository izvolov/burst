#include <burst/container/make_forward_list.hpp>
#include <burst/container/make_vector.hpp>
#include <burst/range/take_n.hpp>
#include <utility/io/forward_list.hpp>

#include <doctest/doctest.h>

#include <boost/range/concepts.hpp>
#include <boost/range/iterator_range.hpp>

#include <iterator>

TEST_SUITE("take_n")
{
    TEST_CASE("Откусывает ровно заданное количество элементов")
    {
        const auto items = burst::make_forward_list({1, 2, 3, 4});

        const auto items_to_take = 3;
        const auto taken = burst::take_n(items, items_to_take);

        using std::begin;
        const auto expected =
            boost::make_iterator_range(begin(items), std::next(begin(items), items_to_take));
        CHECK(taken == expected);
    }

    TEST_CASE("Не отслеживает выход за пределы исходного диапазона")
    {
        const auto items = burst::make_vector({'a', 'b', 'c', 'd'});

        const auto items_to_take = 100500;
        const auto taken = burst::take_n(items, items_to_take);

        CHECK(std::distance(taken.begin(), taken.end()) == items_to_take);
    }

    TEST_CASE("Результирующий диапазон ссылается на исходные диапазоны, так что изменение "
        "элементов результирующего диапазона влечёт изменение соответствующих элементов исходного")
    {
        auto items = burst::make_forward_list({1, 2, 3, 4});

        const auto items_to_take = 3;
        auto taken = burst::take_n(items, items_to_take);

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
        const auto taken = burst::take_n(items, items_to_take);

        BOOST_CONCEPT_ASSERT((boost::RandomAccessIterator<decltype(std::begin(taken))>));
    }
}
