#include <burst/container/make_forward_list.hpp>
#include <burst/container/make_list.hpp>
#include <burst/container/make_vector.hpp>
#include <burst/iterator/buffered_chunk_iterator.hpp>
#include <burst/iterator/end_tag.hpp>

#include <doctest/doctest.h>

#include <boost/range/concepts.hpp>
#include <boost/range/iterator_range.hpp>

#include <algorithm>
#include <istream>
#include <iterator>
#include <sstream>
#include <vector>

namespace
{
    template <typename Value>
    auto make_istream_range (std::istream & s)
    {
        return
            boost::make_iterator_range
            (
                std::istream_iterator<Value>(s),
                std::istream_iterator<Value>{}
            );
    }
}

TEST_SUITE("buffered_chunk_iterator")
{
    TEST_CASE("Буферизованный кусочный итератор всегда однопроходный")
    {
        SUBCASE("Входной итератор — произвольного доступа")
        {
            const auto v = burst::make_vector({1, 2, 3});

            auto initial = v.begin();
            BOOST_CONCEPT_ASSERT((boost::RandomAccessIterator<decltype(initial)>));

            auto chunk = burst::make_buffered_chunk_iterator(initial, v.end(), 2);
            BOOST_CONCEPT_ASSERT((boost::InputIterator<decltype(chunk)>));
        }

        SUBCASE("Входной итератор двунаправленный")
        {
            const auto l = burst::make_list({4, 5, 6, 7});

            auto initial = l.begin();
            BOOST_CONCEPT_ASSERT((boost::BidirectionalIterator<decltype(initial)>));

            auto chunk = burst::make_buffered_chunk_iterator(initial, l.end(), 3);
            BOOST_CONCEPT_ASSERT((boost::InputIterator<decltype(chunk)>));
        }

        SUBCASE("Входной итератор однонаправленный")
        {
            const auto fl = burst::make_forward_list({'q', 'w', 'e'});

            auto initial = fl.begin();
            BOOST_CONCEPT_ASSERT((boost::ForwardIterator<decltype(initial)>));

            auto chunk = burst::make_buffered_chunk_iterator(initial, fl.end(), 1);
            BOOST_CONCEPT_ASSERT((boost::InputIterator<decltype(chunk)>));
        }

        SUBCASE("Входной итератор однопроходный")
        {
            std::stringstream stream("1 2 3 4");

            auto initial = std::istream_iterator<int>(stream);
            BOOST_CONCEPT_ASSERT((boost::InputIterator<decltype(initial)>));

            auto end = std::istream_iterator<int>{};
            auto chunk = burst::make_buffered_chunk_iterator(initial, end, 3);
            BOOST_CONCEPT_ASSERT((boost::InputIterator<decltype(chunk)>));
        }
    }

    TEST_CASE("Конец итератора создаётся из его начала с помощью специальной метки-индикатора")
    {
        auto stream = std::stringstream("1 2 3 4 5");

        auto begin = burst::make_buffered_chunk_iterator(make_istream_range<int>(stream), 3);
        auto end = burst::make_buffered_chunk_iterator(burst::iterator::end_tag, begin);

        CHECK(std::next(begin, 2) == end);
    }

    TEST_CASE("Последний кусок может быть нецелым")
    {
        auto stream = std::stringstream("1 2 3 4 5");

        auto first = burst::make_buffered_chunk_iterator(make_istream_range<int>(stream), 3);
        ++first;

        const auto expected = std::vector<int>{4, 5};
        CHECK(*first == expected);
    }

    TEST_CASE("Буферизованный кусочный итератор умеет продвигаться вперёд")
    {
        const auto v = burst::make_list({'a', 'b', 'c', 'd', 'e'});
        auto iterator = burst::make_buffered_chunk_iterator(v.begin(), v.end(), 2);

        ++iterator;
        CHECK(*iterator == burst::make_vector({'c', 'd'}));

        iterator++;
        CHECK(*iterator == burst::make_vector({'e'}));
    }

    TEST_CASE("Буфер может быть изменён")
    {
        auto stream = std::stringstream("a b c d e f g");

        const auto n = 2;
        auto iterator = burst::make_buffered_chunk_iterator(make_istream_range<char>(stream), n);

        std::fill(iterator->begin(), iterator->end(), 'z');

        CHECK(std::count(iterator->begin(), iterator->end(), 'z') == n);
    }

    TEST_CASE("Проход по всем элементам всех блоков перечисляет все элементы исходного диапазона, "
        "причём ровно один раз")
    {
        const auto container = burst::make_vector({9, 8, 7, 6, 5, 4, 3, 2, 1, 0});

        auto begin = burst::make_buffered_chunk_iterator(container, 3);
        auto end = burst::make_buffered_chunk_iterator(burst::iterator::end_tag, begin);

        std::vector<int> encountered;
        std::for_each(begin, end,
            [& encountered] (const auto & range)
            {
                std::copy(range.begin(), range.end(), std::back_inserter(encountered));
            });

        CHECK(encountered == container);
    }
}
