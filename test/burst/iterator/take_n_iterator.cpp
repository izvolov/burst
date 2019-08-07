#include <burst/container/make_forward_list.hpp>
#include <burst/container/make_list.hpp>
#include <burst/container/make_vector.hpp>
#include <burst/iterator/take_n_iterator.hpp>

#include <doctest/doctest.h>

#include <boost/range/concepts.hpp>
#include <boost/range/iterator_range.hpp>

#include <iterator>
#include <sstream>

TEST_SUITE("take_n_iterator")
{
    TEST_CASE("iterator_category_falls_back_to_forward_iterator")
    {
        // Входной итератор — произвольного доступа
        {
            const auto v = burst::make_vector({1, 2, 3});

            auto initial = v.begin();
            BOOST_CONCEPT_ASSERT((boost::RandomAccessIterator<decltype(initial)>));

            auto take = burst::make_take_n_iterator(initial, 2);
            BOOST_CONCEPT_ASSERT((boost::ForwardIterator<decltype(take)>));
        }

        // Входной итератор двунаправленный
        {
            const auto l = burst::make_list({4, 5, 6, 7});

            auto initial = l.begin();
            BOOST_CONCEPT_ASSERT((boost::BidirectionalIterator<decltype(initial)>));

            auto take = burst::make_take_n_iterator(initial, 3);
            BOOST_CONCEPT_ASSERT((boost::ForwardIterator<decltype(take)>));
        }

        // Входной итератор однонаправленный
        {
            const auto fl = burst::make_forward_list({'q', 'w', 'e'});

            auto initial = fl.begin();
            BOOST_CONCEPT_ASSERT((boost::ForwardIterator<decltype(initial)>));

            auto take = burst::make_take_n_iterator(initial, 1);
            BOOST_CONCEPT_ASSERT((boost::ForwardIterator<decltype(take)>));
        }

        // Входной итератор однопроходный
        {
            std::stringstream stream("1 2 3 4");

            auto initial = std::istream_iterator<int>(stream);
            BOOST_CONCEPT_ASSERT((boost::InputIterator<decltype(initial)>));

            auto take = burst::make_take_n_iterator(initial, 3);
            BOOST_CONCEPT_ASSERT((boost::InputIterator<decltype(take)>));
        }
    }

    TEST_CASE("end_is_created_using_special_tag")
    {
        const auto l = burst::make_list({1, 2, 3, 4});

        auto first = burst::make_take_n_iterator(l.begin(), 3);
        auto last = burst::make_take_n_iterator(burst::iterator::end_tag, first);

        CHECK(std::next(first, 3) == last);
    }

    TEST_CASE("takes_exactly_specified_number_of_elements")
    {
        const auto fl = burst::make_forward_list({1, 2, 3, 4});

        const auto items_to_take = 2;
        auto first = burst::make_take_n_iterator(fl.begin(), items_to_take);
        auto last = burst::make_take_n_iterator(burst::iterator::end_tag, first);

        CHECK
        (
            boost::make_iterator_range(fl.begin(), std::next(fl.begin(), items_to_take)) ==
            boost::make_iterator_range(first, last)
        );
    }
}
