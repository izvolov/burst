#include <burst/container/make_forward_list.hpp>
#include <burst/container/make_vector.hpp>
#include <burst/iterator/cache_iterator.hpp>

#include <doctest/doctest.h>

#include <boost/iterator/transform_iterator.hpp>

#include <iterator>
#include <type_traits>

TEST_SUITE("cache_iterator")
{
    TEST_CASE("iterator_returning_an_lvalue_reference_is_forwarded")
    {
        const auto items = burst::make_vector({1, 2, 3});

        auto rvalue_cached_iterator = burst::make_cache_iterator(items.begin());

        CHECK(std::is_same<decltype(rvalue_cached_iterator), decltype(items.begin())>::value);

        const auto iterator = items.begin();
        auto lvalue_cached_iterator = burst::make_cache_iterator(iterator);

        CHECK(std::is_same<decltype(lvalue_cached_iterator), decltype(items.begin())>::value);
    }

    TEST_CASE("raw_pointer_is_forwarded")
    {
        int items[] = {1, 2, 3};

        auto cached_iterator = burst::make_cache_iterator(items);

        CHECK(std::is_same<decltype(cached_iterator), decltype(std::begin(items))>::value);
    }

    TEST_CASE("is_created_when_wrapped_iterator_returns_by_value")
    {
        const auto items = burst::make_vector({1, 2, 3});
        auto transformed_iterator = boost::make_transform_iterator(items.begin(), [] (auto x) { return x; });

        auto cached_iterator = burst::make_cache_iterator(transformed_iterator);

        CHECK(not std::is_same<decltype(cached_iterator), decltype(items.begin())>::value);
    }

    TEST_CASE("returns_lvalue_reference_to_const_when_dereferenced")
    {
        const auto items = burst::make_vector({1, 2, 3});
        auto transformed_iterator = boost::make_transform_iterator(items.begin(), [] (auto x) { return x; });

        auto cached_iterator = burst::make_cache_iterator(transformed_iterator);

        CHECK
        (
            std::is_same
            <
                std::iterator_traits<decltype(cached_iterator)>::reference,
                const decltype(items)::value_type &
            >
            ::value
        );
    }

    TEST_CASE("does_not_store_value_on_initialization")
    {
        const auto items = burst::make_vector({1, 2, 3});

        auto call_count = 0ul;
        auto transformed_iterator =
            boost::make_transform_iterator(items.begin(),
                [& call_count] (auto x)
                {
                    ++call_count;
                    return x;
                });

        auto cached_iterator = burst::make_cache_iterator(transformed_iterator);
        static_cast<void>(cached_iterator);

        CHECK(call_count == 0);
    }

    TEST_CASE("stores_value_on_first_dereferencing")
    {
        const auto items = burst::make_vector({1, 2, 3});

        auto call_count = 0ul;
        auto transformed_iterator =
            boost::make_transform_iterator(items.begin(),
                [& call_count] (auto x)
                {
                    ++call_count;
                    return x;
                });

        auto cached_iterator = burst::make_cache_iterator(transformed_iterator);

        REQUIRE(call_count == 0);
        *cached_iterator;
        CHECK(call_count == 1);
    }

    TEST_CASE("does_not_recalculate_stored_value_on_repeating_dereferencing")
    {
        const auto items = burst::make_vector({1.1, 2.2, 3.3});

        auto call_count = 0ul;
        auto transformed_iterator =
            boost::make_transform_iterator(items.begin(),
                [& call_count] (auto x)
                {
                    ++call_count;
                    return x;
                });

        auto cached_iterator = burst::make_cache_iterator(transformed_iterator);

        REQUIRE(call_count == 0);
        *cached_iterator;
        REQUIRE(call_count == 1);
        *cached_iterator;
        CHECK(call_count == 1);
    }

    TEST_CASE("drops_stored_value_when_incremented")
    {
        const auto items = burst::make_forward_list({1.1, 2.2, 3.3});

        auto call_count = 0ul;
        auto transformed_iterator =
            boost::make_transform_iterator(items.begin(),
                [& call_count] (auto x)
                {
                    ++call_count;
                    return x;
                });

        auto cached_iterator = burst::make_cache_iterator(transformed_iterator);

        REQUIRE(call_count == 0);
        *cached_iterator;
        REQUIRE(call_count == 1);
        ++cached_iterator;
        *cached_iterator;
        CHECK(call_count == 2);
    }

    TEST_CASE("drops_stored_value_when_advanced")
    {
        const auto items = burst::make_vector({1.1, 2.2, 3.3});

        auto call_count = 0ul;
        auto transformed_iterator =
            boost::make_transform_iterator(items.begin(),
                [& call_count] (auto x)
                {
                    ++call_count;
                    return x;
                });

        auto cached_iterator = burst::make_cache_iterator(transformed_iterator);

        REQUIRE(call_count == 0);
        *cached_iterator;
        REQUIRE(call_count == 1);
        std::advance(cached_iterator, 2);
        *cached_iterator;
        CHECK(call_count == 2);
    }

    TEST_CASE("preserves_wrapped_iterator_category")
    {
        const auto items = burst::make_forward_list({1.1, 2.2, 3.3});

        auto call_count = 0ul;
        auto transformed_iterator =
            boost::make_transform_iterator(items.begin(),
                [& call_count] (auto x)
                {
                    ++call_count;
                    return x;
                });

        auto cached_iterator = burst::make_cache_iterator(transformed_iterator);
        CHECK
        (
            std::is_same
            <
                std::iterator_traits<decltype(items.begin())>::iterator_category,
                std::iterator_traits<decltype(cached_iterator)>::iterator_category
            >
            ::value
        );
    }
}
