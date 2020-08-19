#include <burst/container/make_forward_list.hpp>
#include <burst/container/make_list.hpp>
#include <burst/container/make_vector.hpp>
#include <burst/iterator/take_at_most_iterator.hpp>

#include <doctest/doctest.h>

#include <boost/range/concepts.hpp>
#include <boost/range/iterator_range.hpp>

#include <iterator>
#include <sstream>

TEST_SUITE("take_at_most_iterator")
{
    TEST_CASE("Категория откусывающего итератора не сильнее однонаправленной")
    {
        SUBCASE("Входной итератор — произвольного доступа")
        {
            const auto v = burst::make_vector({1, 2, 3});

            auto initial = v.begin();
            BOOST_CONCEPT_ASSERT((boost::RandomAccessIterator<decltype(initial)>));

            auto take = burst::make_take_at_most_iterator(initial, v.end(), 2);
            BOOST_CONCEPT_ASSERT((boost::ForwardIterator<decltype(take)>));
        }

        SUBCASE("Входной итератор двунаправленный")
        {
            const auto l = burst::make_list({4, 5, 6, 7});

            BOOST_CONCEPT_ASSERT((boost::BidirectionalRangeConcept<decltype(l)>));

            auto take = burst::make_take_at_most_iterator(l, 3);
            BOOST_CONCEPT_ASSERT((boost::ForwardIterator<decltype(take)>));
        }

        SUBCASE("Входной итератор однонаправленный")
        {
            const auto fl = burst::make_forward_list({'q', 'w', 'e'});

            BOOST_CONCEPT_ASSERT((boost::ForwardRangeConcept<decltype(fl)>));

            auto take = burst::make_take_at_most_iterator(fl, 1);
            BOOST_CONCEPT_ASSERT((boost::ForwardIterator<decltype(take)>));
        }

        SUBCASE("Входной итератор однопроходный")
        {
            std::stringstream stream("1 2 3 4");

            auto initial = std::istream_iterator<int>(stream);
            BOOST_CONCEPT_ASSERT((boost::InputIterator<decltype(initial)>));

            auto end = std::istream_iterator<int>{};
            auto take = burst::make_take_at_most_iterator(initial, end, 3);
            BOOST_CONCEPT_ASSERT((boost::InputIterator<decltype(take)>));
        }
    }

    TEST_CASE("Конец откусывающего итератора создаётся из его начала с помощью специальной "
        "метки-индикатора")
    {
        const auto l = burst::make_list({1, 2, 3, 4});

        auto first = burst::make_take_at_most_iterator(l, 3);
        auto last = burst::make_take_at_most_iterator(burst::iterator::end_tag, first);

        CHECK(std::next(first, 3) == last);
    }

    TEST_CASE("Откусывает заданное количество элементов")
    {
        const auto fl = burst::make_forward_list({1, 2, 3, 4});

        const auto items_to_take = 2;
        auto first = burst::make_take_at_most_iterator(fl.begin(), fl.end(), items_to_take);
        auto last = burst::make_take_at_most_iterator(burst::iterator::end_tag, first);

        CHECK
        (
            boost::make_iterator_range(first, last) ==
            boost::make_iterator_range(fl.begin(), std::next(fl.begin(), items_to_take))
        );
    }

    TEST_CASE("Откусывает не больше, чем есть в исходном диапазоне")
    {
        const auto fl = burst::make_forward_list({1, 2, 3, 4});

        const auto items_to_take = 100500;
        auto first = burst::make_take_at_most_iterator(fl, items_to_take);
        auto last = burst::make_take_at_most_iterator(burst::iterator::end_tag, first);

        CHECK(boost::make_iterator_range(first, last) == fl);
    }
}
