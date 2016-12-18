#include <burst/container/make_forward_list.hpp>
#include <burst/container/make_vector.hpp>
#include <burst/iterator/cache_iterator.hpp>

#include <boost/iterator/transform_iterator.hpp>
#include <boost/test/unit_test.hpp>

#include <iterator>
#include <type_traits>

BOOST_AUTO_TEST_SUITE(cache_iterator)
    BOOST_AUTO_TEST_CASE(iterator_returning_an_lvalue_reference_is_forwarded)
    {
        const auto items = burst::make_vector({1, 2, 3});

        auto rvalue_cached_iterator = burst::make_cache_iterator(items.begin());

        BOOST_CHECK((std::is_same<decltype(rvalue_cached_iterator), decltype(items.begin())>::value));

        const auto iterator = items.begin();
        auto lvalue_cached_iterator = burst::make_cache_iterator(iterator);

        BOOST_CHECK((std::is_same<decltype(lvalue_cached_iterator), decltype(items.begin())>::value));
    }

    BOOST_AUTO_TEST_CASE(raw_pointer_is_forwarded)
    {
        int items[] = {1, 2, 3};

        auto cached_iterator = burst::make_cache_iterator(items);

        BOOST_CHECK((std::is_same<decltype(cached_iterator), decltype(std::begin(items))>::value));
    }

    BOOST_AUTO_TEST_CASE(is_created_when_wrapped_iterator_returns_by_value)
    {
        const auto items = burst::make_vector({1, 2, 3});
        auto transformed_iterator = boost::make_transform_iterator(items.begin(), [] (auto x) { return x; });

        auto cached_iterator = burst::make_cache_iterator(transformed_iterator);

        BOOST_CHECK((not std::is_same<decltype(cached_iterator), decltype(items.begin())>::value));
    }

    BOOST_AUTO_TEST_CASE(returns_lvalue_reference_to_const_when_dereferenced)
    {
        const auto items = burst::make_vector({1, 2, 3});
        auto transformed_iterator = boost::make_transform_iterator(items.begin(), [] (auto x) { return x; });

        auto cached_iterator = burst::make_cache_iterator(transformed_iterator);

        BOOST_CHECK
        ((
            std::is_same
            <
                std::iterator_traits<decltype(cached_iterator)>::reference,
                const decltype(items)::value_type &
            >
            ::value
        ));
    }

    BOOST_AUTO_TEST_CASE(does_not_store_value_on_initialization)
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

        BOOST_CHECK_EQUAL(call_count, 0);
    }

    BOOST_AUTO_TEST_CASE(stores_value_on_first_dereferencing)
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

        BOOST_REQUIRE_EQUAL(call_count, 0);
        *cached_iterator;
        BOOST_CHECK_EQUAL(call_count, 1);
    }

    BOOST_AUTO_TEST_CASE(does_not_recalculate_stored_value_on_repeating_dereferencing)
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

        BOOST_REQUIRE_EQUAL(call_count, 0);
        *cached_iterator;
        BOOST_REQUIRE_EQUAL(call_count, 1);
        *cached_iterator;
        BOOST_CHECK_EQUAL(call_count, 1);
    }

    BOOST_AUTO_TEST_CASE(drops_stored_value_when_incremented)
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

        BOOST_REQUIRE_EQUAL(call_count, 0);
        *cached_iterator;
        BOOST_REQUIRE_EQUAL(call_count, 1);
        ++cached_iterator;
        *cached_iterator;
        BOOST_CHECK_EQUAL(call_count, 2);
    }

    BOOST_AUTO_TEST_CASE(drops_stored_value_when_advanced)
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

        BOOST_REQUIRE_EQUAL(call_count, 0);
        *cached_iterator;
        BOOST_REQUIRE_EQUAL(call_count, 1);
        std::advance(cached_iterator, 2);
        *cached_iterator;
        BOOST_CHECK_EQUAL(call_count, 2);
    }

    BOOST_AUTO_TEST_CASE(preserves_wrapped_iterator_category)
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
        BOOST_CHECK
        ((
            std::is_same
            <
                std::iterator_traits<decltype(items.begin())>::iterator_category,
                std::iterator_traits<decltype(cached_iterator)>::iterator_category
            >
            ::value
        ));
    }
BOOST_AUTO_TEST_SUITE_END()
