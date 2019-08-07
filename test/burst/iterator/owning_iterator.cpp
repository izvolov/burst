#include <burst/container/make_forward_list.hpp>
#include <burst/container/make_list.hpp>
#include <burst/container/make_vector.hpp>
#include <burst/iterator/owning_iterator.hpp>

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
    TEST_CASE("owning_iterator_end_is_created_using_special_tag")
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

    TEST_CASE_TEMPLATE("preserves_container_iterator_category", container,
        std::forward_list<int>, std::list<int>, std::vector<int>)
    {
        using container_iterator_category =
            typename std::iterator_traits
            <
                typename container::iterator
            >
            ::iterator_category;

        using owning_iterator_category =
            typename std::iterator_traits
            <
                burst::owning_iterator<container>
            >
            ::iterator_category;

        CHECK(std::is_same<container_iterator_category, owning_iterator_category>::value);
    }

    TEST_CASE("preserves_container_iterator_concept")
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

    TEST_CASE("every_copy_of_the_owning_iterator_iterates_exactly_the_same_container_as_the_original_one")
    {
        auto original = burst::make_owning_iterator(burst::make_vector({1, 2, 3}));
        auto copy = original;

        *original = 17;

        CHECK(*copy == 17);
    }
}
