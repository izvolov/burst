#include <string>
#include <vector>

#include <boost/test/unit_test.hpp>

#include <iostream>
#include <burst/container/dynamic_tuple.hpp>
#include <test/output.hpp>

BOOST_AUTO_TEST_SUITE(dynamic_tuple)
    struct X
    {
        int a = 0;
        std::vector<int> b = std::vector<int>{0};
    };

    struct dummy
    {
        dummy ()
        {
            ++instances_count;
        }

        dummy (const dummy &)
        {
            ++instances_count;
        }

        dummy (dummy &&)
        {
            ++instances_count;
        }

        ~dummy ()
        {
            --instances_count;
        }

        static int instances_count;
    };

    int dummy::instances_count = 0;

    BOOST_AUTO_TEST_CASE(dynamic_tuple_can_be_created_from_an_arbitrary_set_of_types)
    {
        burst::dynamic_tuple t(std::string("123"), 42, true, X{17, std::vector<int>{1, 2, 3}});
        BOOST_CHECK_EQUAL(t.size(), 4);
        BOOST_CHECK_EQUAL(t.get<std::string>(0), std::string("123"));
        BOOST_CHECK_EQUAL(t.get<int>(1), 42);
        BOOST_CHECK_EQUAL(t.get<bool>(2), true);
        BOOST_CHECK_EQUAL(t.get<X>(3).a, 17);
        BOOST_CHECK_EQUAL(t.get<X>(3).b, (std::vector<int>{1, 2, 3}));
    }

    BOOST_AUTO_TEST_CASE(volume_of_dynamic_tuple_is_between_sum_of_sizes_and_sizes_plus_alignments_of_inlying_types)
    {
        const auto some_vector = std::vector<std::size_t>{};
        const auto some_integer = 42;
        const auto some_struct = X{};
        const auto some_floating = 3.14;

        burst::dynamic_tuple t(some_vector, some_integer, some_struct, some_floating);
        BOOST_CHECK_GE
        (
            t.volume(),
            sizeof(some_vector) +
            sizeof(some_integer) +
            sizeof(some_struct) +
            sizeof(some_floating)
        );

        BOOST_CHECK_LE
        (
            t.volume(),
            sizeof(some_vector) + alignof(decltype(some_vector)) +
            sizeof(some_integer) + alignof(decltype(some_integer)) +
            sizeof(some_struct) + alignof(decltype(some_struct)) +
            sizeof(some_floating) + alignof(decltype(some_floating))
        );
    }

    BOOST_AUTO_TEST_CASE(size_of_dynamic_tuple_is_equal_to_count_of_inlying_objects)
    {
        const auto some_vector = std::vector<std::size_t>{};
        const auto some_integer = 42;
        const auto some_struct = X{};
        const auto some_floating = 3.14;

        burst::dynamic_tuple t(some_vector, some_integer, some_struct, some_floating);
        BOOST_CHECK_EQUAL(t.size(), 4);
    }

    BOOST_AUTO_TEST_CASE(arbitrary_objects_can_be_pushed_back)
    {
        burst::dynamic_tuple t;

        t.push_back(X{17, std::vector<int>{1, 2, 3}});
        t.push_back(true);
        t.push_back(std::string("123"));

        BOOST_CHECK_EQUAL(t.size(), 3);
        BOOST_CHECK_EQUAL(t.get<X>(0).a, 17);
        BOOST_CHECK_EQUAL(t.get<X>(0).b, (std::vector<int>{1, 2, 3}));
        BOOST_CHECK_EQUAL(t.get<bool>(1), true);
        BOOST_CHECK_EQUAL(t.get<std::string>(2), std::string("123"));
    }

    BOOST_AUTO_TEST_CASE(clear_method_destructs_inlying_objects)
    {
        burst::dynamic_tuple t;
        t.push_back(dummy{});

        BOOST_REQUIRE_EQUAL(dummy::instances_count, 1);
        t.clear();

        BOOST_CHECK_EQUAL(dummy::instances_count, 0);
    }

    BOOST_AUTO_TEST_CASE(dynamic_tuple_is_empty_after_clear)
    {
        burst::dynamic_tuple t(1, 3.14, true, std::string("qwe"));

        BOOST_REQUIRE_EQUAL(t.size(), 4);
        BOOST_REQUIRE(not t.empty());
        t.clear();

        BOOST_CHECK_EQUAL(t.size(), 0);
        BOOST_CHECK(t.empty());
    }

    BOOST_AUTO_TEST_CASE(capacity_does_not_change_after_clear)
    {
        burst::dynamic_tuple t(1, 3.14, true, std::string("qwe"));

        const auto capacity = t.capacity();
        BOOST_REQUIRE_GT(capacity, 0);
        t.clear();

        BOOST_CHECK_EQUAL(t.capacity(), capacity);
    }

    struct kamikaze
    {
        kamikaze ()
        {
            ++instances_count;
        }

        kamikaze (const kamikaze &) = delete;

        kamikaze (kamikaze &&)
        {
            if (instances_count > 3)
            {
                throw std::runtime_error(u8"Привет!");
            }
            ++instances_count;
        }

        ~kamikaze ()
        {
            --instances_count;
        }

        static int instances_count;
    };

    int kamikaze::instances_count = 0;

    BOOST_AUTO_TEST_CASE(does_not_leak_when_inserted_element_throws)
    {
        try
        {
            burst::dynamic_tuple t(kamikaze{}, kamikaze{});
        }
        catch (std::runtime_error &)
        {
            BOOST_CHECK_EQUAL(kamikaze::instances_count, 0);
        }
    }
BOOST_AUTO_TEST_SUITE_END()
