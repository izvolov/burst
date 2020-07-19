#include <burst/container/make_deque.hpp>
#include <burst/container/make_forward_list.hpp>
#include <burst/container/make_vector.hpp>
#include <burst/iterator/join_iterator.hpp>
#include <burst/range/make_range_vector.hpp>

#include <doctest/doctest.h>

#include <boost/range/iterator_range.hpp>

#include <iterator>

TEST_SUITE("join_iterator")
{
    TEST_CASE("Конец итератора склейки создаётся из его начала с помощью специальной "
        "метки-индикатора")
    {
        auto  first = {500, 100};
        auto second = {600, 200};
        auto ranges = burst::make_range_vector(first, second);

        auto joined_begin = burst::make_join_iterator(ranges);
        auto   joined_end = burst::make_join_iterator(burst::iterator::end_tag, joined_begin);

        auto expected_collection = {500, 100, 600, 200};
        CHECK(boost::make_iterator_range(joined_begin, joined_end) == expected_collection);
    }

    TEST_CASE("Итератор склейки, созданный из диапазонов произвольного доступа — "
        "итератор произвольного доступа")
    {
        auto  first = {3, 2};
        auto second = {1, 0};
        auto ranges = burst::make_range_vector(first, second);

        auto joined_begin = burst::make_join_iterator(ranges);
        auto joined_end   = burst::make_join_iterator(burst::iterator::end_tag, joined_begin);

        BOOST_CONCEPT_ASSERT((boost::RandomAccessIterator<decltype(joined_begin)>));
        BOOST_CONCEPT_ASSERT((boost::RandomAccessIterator<decltype(joined_end)>));
    }

    TEST_CASE("Итератор склейки, созданный из диапазонов произвольного доступа (с помощью "
        "кортежа) — итератор произвольного доступа")
    {
        auto  first = burst::make_vector({3, 2});
        auto second = burst::make_deque({1, 0});

        auto joined_begin = burst::make_join_iterator(std::tie(first, second));
        auto joined_end   = burst::make_join_iterator(burst::iterator::end_tag, joined_begin);

        BOOST_CONCEPT_ASSERT((boost::RandomAccessIterator<decltype(joined_begin)>));
        BOOST_CONCEPT_ASSERT((boost::RandomAccessIterator<decltype(joined_end)>));
    }

    TEST_CASE("Уменьшение итератора склейки произвольного доступа допустимо")
    {
        auto  first = {3, 2};
        auto second = {1, 0};
        auto ranges = burst::make_range_vector(first, second);

        auto joined_begin = burst::make_join_iterator(ranges);
        auto joined_end = burst::make_join_iterator(burst::iterator::end_tag, joined_begin);
        --joined_end;

        CHECK(*joined_end == 0);
    }

    TEST_CASE("Копия однопроходного итератора склейки равна оригиналу")
    {
        auto first = burst::make_forward_list({100, 50});
        auto second = burst::make_forward_list({70, 30});
        auto ranges = burst::make_range_vector(first, second);

        auto join_iterator = burst::make_join_iterator(ranges);

        auto join_iterator_copy = join_iterator;
        CHECK(join_iterator_copy == join_iterator);
    }

    TEST_CASE("Копия итератора склейки произвольного доступа равна оригиналу")
    {
        auto first = burst::make_vector({100, 50});
        auto second = burst::make_vector({70, 30});
        auto ranges = burst::make_range_vector(first, second);

        auto join_iterator = burst::make_join_iterator(ranges);

        auto join_iterator_copy = join_iterator;
        CHECK(join_iterator_copy == join_iterator);
    }

    TEST_CASE("Изменение копии итератора склейки произвольного доступа не изменяет оригинал")
    {
        auto first = burst::make_vector({100, 50});
        auto second = burst::make_vector({70, 30});
        auto ranges = burst::make_range_vector(first, second);

        auto join_iterator = burst::make_join_iterator(ranges);

        auto join_iterator_copy = join_iterator;
        join_iterator_copy += 2;

        CHECK(*join_iterator_copy == 70);
        CHECK(*join_iterator == 100);
    }

    TEST_CASE("Изменение копии однопроходного итератора склейки изменяет оригинал")
    {
        auto first = burst::make_forward_list({100, 50});
        auto second = burst::make_forward_list({70, 30});
        auto ranges = burst::make_range_vector(first, second);

        auto join_iterator = burst::make_join_iterator(ranges);

        auto join_iterator_copy = join_iterator;
        ++join_iterator_copy;

        CHECK(*join_iterator_copy == 50);
        CHECK(*join_iterator == 50);
    }

    TEST_CASE("Произвольный доступ между границами склейки происходит бесшовно")
    {
        auto  first = {3, 2};
        auto second = {1, 0};
        auto ranges = burst::make_range_vector(first, second);

        auto joined_begin = burst::make_join_iterator(ranges);
        auto joined_end   = burst::make_join_iterator(burst::iterator::end_tag, joined_begin);

        joined_begin += 2;
        CHECK(*joined_begin == 1);
        CHECK(joined_end - joined_begin == 2);

        joined_begin -= 2;
        CHECK(*joined_begin == 3);
        CHECK(joined_end - joined_begin == 4);
    }
}
