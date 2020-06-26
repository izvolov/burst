#include <burst/container/make_forward_list.hpp>
#include <burst/container/make_list.hpp>
#include <burst/container/make_vector.hpp>
#include <burst/iterator/owning_iterator.hpp>
#include <burst/type_traits/iterator_category.hpp>

#include <doctest/doctest.h>

#include <boost/mpl/vector.hpp>
#include <boost/range/concepts.hpp>
#include <boost/range/iterator_range.hpp>

#include <forward_list>
#include <iterator>
#include <list>
#include <type_traits>
#include <vector>

TEST_SUITE("owning_iterator")
{
    TEST_CASE("Конец владеющего итератора создаётся из его начала с помощью специальной "
        "метки-индикатора")
    {
        const auto owned_begin = burst::make_owning_iterator(burst::make_vector({1, 2, 3}));
        const auto owned_end = burst::make_owning_iterator(burst::iterator::end_tag, owned_begin);

        const auto expected = {1, 2, 3};
        CHECK(boost::make_iterator_range(owned_begin, owned_end) == expected);
    }

    using containers =
        boost::mpl::vector
        <
            std::forward_list<int>,
            std::list<int>,
            std::vector<int>
        >;

    TEST_CASE_TEMPLATE("Наследует категорию итераторов контейнера", container,
        std::forward_list<int>, std::list<int>, std::vector<int>)
    {
        using container_iterator_category =
            burst::iterator_category_t<typename container::iterator>;
        using owning_iterator_category =
            burst::iterator_category_t<burst::owning_iterator<container>>;

        CHECK(std::is_same<container_iterator_category, owning_iterator_category>::value);
    }

    TEST_CASE("Наследует модель итератора контейнера")
    {
        // Однонаправленный итератор
        {
            auto owned_begin = burst::make_owning_iterator(burst::make_forward_list({1, 2, 3}));
            BOOST_CONCEPT_ASSERT((boost::ForwardIterator<decltype(owned_begin)>));
        }
        // Двунаправленный итератор
        {
            auto owned_begin = burst::make_owning_iterator(burst::make_list({1, 2, 3}));
            BOOST_CONCEPT_ASSERT((boost::BidirectionalIterator<decltype(owned_begin)>));
        }
        // Итератор произвольного доступа
        {
            auto owned_begin = burst::make_owning_iterator(burst::make_vector({1, 2, 3}));
            BOOST_CONCEPT_ASSERT((boost::RandomAccessIterator<decltype(owned_begin)>));
        }
    }

    TEST_CASE("Каждая копия владеющего итератора ссылается на один и тот же экземпляр контейнера")
    {
        auto original = burst::make_owning_iterator(burst::make_vector({1, 2, 3}));
        auto copy = original;

        *original = 17;

        CHECK(*copy == 17);
    }

    TEST_CASE("Владеющий итератор произвольного доступа умеет продвигаться на N элементов вперёд "
        "и назад")
    {
        auto iterator = burst::make_owning_iterator(burst::make_vector({'a', 'b', 'c', 'd'}));

        iterator += 3;
        CHECK(*iterator == 'd');

        iterator -= 2;
        CHECK(*iterator == 'b');
    }

    TEST_CASE("Владеющий итератор произвольного доступа допускает вычисление расстояния при "
        "помощи разности")
    {
        const auto one = burst::make_owning_iterator(burst::make_vector({'a', 'b', 'c', 'd'}));

        const auto another = std::next(one, 3);
        CHECK(another - one == 3);
    }

    TEST_CASE("Двунаправленный владеющий итератор умеет продвигаться вперёд и назад")
    {
        auto iterator = burst::make_owning_iterator(burst::make_list({'a', 'b', 'c', 'd'}));

        ++iterator;
        CHECK(*iterator == 'b');

        iterator++;
        CHECK(*iterator == 'c');

        --iterator;
        CHECK(*iterator == 'b');

        iterator--;
        CHECK(*iterator == 'a');
    }
}
