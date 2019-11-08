#include <burst/container/make_forward_list.hpp>
#include <burst/container/make_vector.hpp>
#include <burst/iterator/cache_iterator.hpp>

#include <doctest/doctest.h>

#include <boost/iterator/transform_iterator.hpp>

#include <iterator>
#include <type_traits>

TEST_SUITE("cache_iterator")
{
    TEST_CASE("Итератор, возвращающий ссылку на lvalue, не нуждается в кэшировании, а потому "
        "пробрасывается без изменения")
    {
        const auto items = burst::make_vector({1, 2, 3});

        auto rvalue_cached_iterator = burst::make_cache_iterator(items.begin());

        CHECK(std::is_same<decltype(rvalue_cached_iterator), decltype(items.begin())>::value);

        const auto iterator = items.begin();
        auto lvalue_cached_iterator = burst::make_cache_iterator(iterator);

        CHECK(std::is_same<decltype(lvalue_cached_iterator), decltype(items.begin())>::value);
    }

    TEST_CASE("Голый указатель пробрасывается")
    {
        int items[] = {1, 2, 3};

        auto cached_iterator = burst::make_cache_iterator(items);

        CHECK(std::is_same<decltype(cached_iterator), decltype(std::begin(items))>::value);
    }

    TEST_CASE("Создаётся только тогда, когда исходный итератор возвращает по значению")
    {
        const auto items = burst::make_vector({1, 2, 3});
        auto transformed_iterator = boost::make_transform_iterator(items.begin(), [] (auto x) { return x; });

        auto cached_iterator = burst::make_cache_iterator(transformed_iterator);

        CHECK(not std::is_same<decltype(cached_iterator), decltype(items.begin())>::value);
    }

    TEST_CASE("При разыменовании возвращает lvalue-ссылку на константу")
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

    TEST_CASE("Не разыменовывает исходный итератор при инициализации")
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

    TEST_CASE("Кэширует значение при первом явном разыменовании")
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

    TEST_CASE("Не пересчитывает закэшированное значение при повторном разыменовании")
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

    TEST_CASE("Сбрасывает закэшированное значение при инкрементном продвижении")
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

    TEST_CASE("Сбрасывает закэшированное значение при продвижении")
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

    TEST_CASE("Наследует категорию обёрнутого итератора")
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
