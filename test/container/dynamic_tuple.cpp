#include <burst/container/dynamic_tuple.hpp>
#include <iostream>
#include <test/output.hpp>

#include <boost/test/unit_test.hpp>

#include <string>
#include <vector>

BOOST_AUTO_TEST_SUITE(dynamic_tuple)
    struct X
    {
        int a = 0;
        std::vector<int> b = std::vector<int>{0};
    };

    struct dummy
    {
        dummy ():
            alive(true)
        {
            ++instances_count;
        }

        dummy (const dummy &):
            alive(true)
        {
            ++instances_count;
        }

        dummy (dummy &&):
            alive(true)
        {
            ++instances_count;
        }

        ~dummy ()
        {
            if (alive)
            {
                alive = false;
            }
            else
            {
                throw std::runtime_error(u8"Двойное уничтожение!");
            }
            --instances_count;
        }

        bool alive;
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

    BOOST_AUTO_TEST_CASE(newly_created_tuple_with_no_elements_is_empty)
    {
        const auto t = burst::dynamic_tuple{};
        BOOST_CHECK(t.empty());
    }

    BOOST_AUTO_TEST_CASE(size_of_newly_created_tuple_with_no_elements_is_zero)
    {
        const auto t = burst::dynamic_tuple{};
        BOOST_CHECK_EQUAL(t.size(), 0);
    }

    BOOST_AUTO_TEST_CASE(volume_of_newly_created_tuple_with_no_elements_is_zero)
    {
        const auto t = burst::dynamic_tuple{};
        BOOST_CHECK_EQUAL(t.volume(), 0);
    }

    BOOST_AUTO_TEST_CASE(capacity_of_newly_created_tuple_with_no_elements_is_zero)
    {
        const auto t = burst::dynamic_tuple{};
        BOOST_CHECK_EQUAL(t.capacity(), 0);
    }

    BOOST_AUTO_TEST_CASE(newly_created_tuple_with_some_elements_is_not_empty)
    {
        const auto t = burst::dynamic_tuple(42, 3.14);
        BOOST_CHECK(not t.empty());
    }

    BOOST_AUTO_TEST_CASE(size_of_newly_created_tuple_with_some_elements_is_amount_of_those_elements)
    {
        const auto t = burst::dynamic_tuple(std::string("123"), 42, 2.71, false);
        BOOST_CHECK_EQUAL(t.size(), 4);
    }

    BOOST_AUTO_TEST_CASE(volume_of_newly_created_tuple_with_some_elements_is_equal_to_size_of_respective_struct)
    {
        const auto t = burst::dynamic_tuple(42, 3.14, std::string("123"));

        using respective = struct{int a; double b; std::string c;};
        BOOST_CHECK_EQUAL(t.volume(), sizeof(respective));
    }

    BOOST_AUTO_TEST_CASE(capacity_of_newly_created_tuple_with_some_elements_is_greater_than_or_equal_to_volume)
    {
        const auto t = burst::dynamic_tuple(std::vector<std::size_t>{}, 42, X{}, 3.14);
        BOOST_CHECK_GE(t.capacity(), t.volume());
    }

    BOOST_AUTO_TEST_CASE(reserving_less_space_than_already_present_does_nothing)
    {
        auto t = burst::dynamic_tuple(true, 42, std::string("qwe"));
        auto old_capacity = t.capacity();

        t.reserve(t.capacity() / 2);

        BOOST_CHECK_EQUAL(t.capacity(), old_capacity);
    }

    BOOST_AUTO_TEST_CASE(reserving_space_equal_to_current_capacity_does_nothing)
    {
        auto t = burst::dynamic_tuple(true, 42, std::string("qwe"));
        auto old_capacity = t.capacity();

        t.reserve(t.capacity());

        BOOST_CHECK_EQUAL(t.capacity(), old_capacity);
    }

    BOOST_AUTO_TEST_CASE(new_capacity_of_empty_tuple_is_greater_or_equal_to_value_of_reserved_space)
    {
        auto t = burst::dynamic_tuple{};

        t.reserve(10);

        BOOST_CHECK_GE(t.capacity(), 10);
    }

    BOOST_AUTO_TEST_CASE(new_capacity_of_non_empty_tuple_is_greater_or_equal_to_value_of_reserved_space)
    {
        auto t = burst::dynamic_tuple(false, 3.14, 42);

        auto desired_capacity = t.capacity() + 10;
        t.reserve(desired_capacity);

        BOOST_CHECK_GE(t.capacity(), desired_capacity);
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

    BOOST_AUTO_TEST_CASE(volume_is_zero_after_clear)
    {
        burst::dynamic_tuple t(1, 3.14, true, std::string("qwe"));

        t.clear();

        BOOST_CHECK_EQUAL(t.volume(), 0);
    }

    BOOST_AUTO_TEST_CASE(size_is_zero_after_clear)
    {
        burst::dynamic_tuple t(1, 3.14, true, std::string("qwe"));

        t.clear();

        BOOST_CHECK_EQUAL(t.size(), 0);
    }

    BOOST_AUTO_TEST_CASE(dynamic_tuple_is_empty_after_clear)
    {
        burst::dynamic_tuple t(1, 3.14, true, std::string("qwe"));

        t.clear();

        BOOST_CHECK(t.empty());
    }

    BOOST_AUTO_TEST_CASE(capacity_does_not_change_after_clear)
    {
        burst::dynamic_tuple t(1, 3.14, true, std::string("qwe"));
        const auto capacity = t.capacity();

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
            if (instances_count > 4)
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
        BOOST_REQUIRE_THROW(burst::dynamic_tuple(kamikaze{}, kamikaze{}, kamikaze{}), std::runtime_error);
        BOOST_CHECK_EQUAL(kamikaze::instances_count, 0);
    }

    BOOST_AUTO_TEST_CASE(underlying_items_can_be_mutated)
    {
        burst::dynamic_tuple t(true, std::vector<char>{'a', 'b' ,'c'}, 10);

        auto & vector = t.get<std::vector<char>>(1);

        vector.push_back('q');
        BOOST_CHECK_EQUAL(t.get<std::vector<char>>(1), (std::vector<char>{'a', 'b' ,'c', 'q'}));

        vector.clear();
        BOOST_CHECK(t.get<std::vector<char>>(1).empty());
    }

    BOOST_AUTO_TEST_CASE(inlying_objects_move_together_with_dynamic_tuple)
    {
        BOOST_REQUIRE_EQUAL(dummy::instances_count, 0);
        burst::dynamic_tuple one(dummy{});
        BOOST_REQUIRE_EQUAL(dummy::instances_count, 1);

        burst::dynamic_tuple another(std::move(one));
        BOOST_CHECK_EQUAL(dummy::instances_count, 1);

        one = std::move(another);
        BOOST_CHECK_EQUAL(dummy::instances_count, 1);
    }

    struct throw_on_move
    {
        throw_on_move () = default;
        throw_on_move (const throw_on_move &) = default;
        throw_on_move & operator = (const throw_on_move &) = default;
        throw_on_move (throw_on_move &&)
        {
            throw std::runtime_error(u8"Пока!");
        }
        throw_on_move & operator = (throw_on_move &&) = default;
        ~throw_on_move () = default;
    };

    BOOST_AUTO_TEST_CASE(does_not_leak_when_inlying_element_throws_being_moved)
    {
        const throw_on_move thrower{};

        BOOST_REQUIRE_EQUAL(dummy::instances_count, 0);
        {
            burst::dynamic_tuple t;
            t.push_back(dummy{});
            t.push_back(thrower);
            BOOST_REQUIRE_EQUAL(dummy::instances_count, 1);

            BOOST_REQUIRE_THROW(t.reserve(t.capacity() + 1), std::runtime_error);
            BOOST_CHECK_EQUAL(dummy::instances_count, 1);
        }
        BOOST_CHECK_EQUAL(dummy::instances_count, 0);
    }

    BOOST_AUTO_TEST_CASE(proper_ctor_dtor_calls_of_inlying_objects_in_dyntuple_copy_constructor)
    {
        BOOST_REQUIRE_EQUAL(dummy::instances_count, 0);
        {
            burst::dynamic_tuple initial(dummy{});

            BOOST_REQUIRE_EQUAL(dummy::instances_count, 1);
            {
                burst::dynamic_tuple copy(initial);
                BOOST_CHECK_EQUAL(dummy::instances_count, 2);
            }
            BOOST_CHECK_EQUAL(dummy::instances_count, 1);
        }
        BOOST_CHECK_EQUAL(dummy::instances_count, 0);
    }

    BOOST_AUTO_TEST_CASE(proper_ctor_dtor_calls_of_inlying_objects_in_dyntuple_copy_assignment)
    {
        BOOST_REQUIRE_EQUAL(dummy::instances_count, 0);
        {
            burst::dynamic_tuple initial(dummy{});

            BOOST_REQUIRE_EQUAL(dummy::instances_count, 1);
            {
                burst::dynamic_tuple copy(dummy{});
                copy = initial;
                BOOST_CHECK_EQUAL(dummy::instances_count, 2);
            }
            BOOST_CHECK_EQUAL(dummy::instances_count, 1);
        }
        BOOST_CHECK_EQUAL(dummy::instances_count, 0);
    }

    BOOST_AUTO_TEST_CASE(copy_construction_is_deep)
    {
        burst::dynamic_tuple initial(std::string("cat"));

        auto copy = burst::dynamic_tuple(initial);
        BOOST_REQUIRE_EQUAL(copy.get<std::string>(0), "cat");

        initial.get<std::string>(0).append("harsis");
        BOOST_CHECK_EQUAL(copy.get<std::string>(0), "cat");
    }

    BOOST_AUTO_TEST_CASE(copy_assignment_is_deep)
    {
        burst::dynamic_tuple initial(std::vector<std::string>(1, "dog"));

        burst::dynamic_tuple copy(std::string("qwerty"));
        copy = initial;
        BOOST_REQUIRE_EQUAL(copy.get<std::vector<std::string>>(0), std::vector<std::string>(1, "dog"));

        initial.get<std::vector<std::string>>(0).push_back("horse");
        BOOST_CHECK_EQUAL(copy.get<std::vector<std::string>>(0), std::vector<std::string>(1, "dog"));
    }

    BOOST_AUTO_TEST_CASE(volume_is_equal_after_copy_assignment)
    {
        burst::dynamic_tuple initial(std::string("cat"), 5, 3.14, std::vector<int>{1, 2});

        burst::dynamic_tuple copy(true, 2.71);
        copy = initial;
        BOOST_CHECK_EQUAL(initial.volume(), copy.volume());
    }

    BOOST_AUTO_TEST_CASE(sizes_are_equal_after_copy_assignment)
    {
        burst::dynamic_tuple initial(std::string("cat"), 5, 3.14, std::vector<int>{1, 2});

        burst::dynamic_tuple copy(17);
        copy = initial;
        BOOST_CHECK_EQUAL(initial.size(), copy.size());
    }

    BOOST_AUTO_TEST_CASE(copy_assignment_of_empty_tuple_results_empty_tuple)
    {
        burst::dynamic_tuple initial;

        burst::dynamic_tuple copy(std::string("123"), 'a');
        copy = initial;
        BOOST_CHECK(copy.empty());
    }

    BOOST_AUTO_TEST_CASE(throws_on_attempt_to_copy_noncopyable_object)
    {
        burst::dynamic_tuple initial(std::make_unique<int>(5));
        BOOST_CHECK_THROW(burst::dynamic_tuple copy(initial), std::runtime_error);
    }

    BOOST_AUTO_TEST_CASE(does_not_leak_when_inlying_element_throws_being_copy_constructed)
    {
        BOOST_REQUIRE_EQUAL(dummy::instances_count, 0);
        {
            burst::dynamic_tuple t(dummy{}, std::make_unique<int>(17));
            BOOST_REQUIRE_EQUAL(dummy::instances_count, 1);

            BOOST_REQUIRE_THROW(burst::dynamic_tuple copy(t), std::runtime_error);
            BOOST_CHECK_EQUAL(dummy::instances_count, 1);
        }
        BOOST_CHECK_EQUAL(dummy::instances_count, 0);
    }

    BOOST_AUTO_TEST_CASE(does_not_leak_when_inlying_element_throws_being_copy_assigned)
    {
        BOOST_REQUIRE_EQUAL(dummy::instances_count, 0);
        {
            burst::dynamic_tuple t(dummy{}, std::make_unique<int>(17));
            burst::dynamic_tuple copy(dummy{});
            BOOST_REQUIRE_EQUAL(dummy::instances_count, 2);

            BOOST_REQUIRE_THROW(copy = t, std::runtime_error);
            BOOST_CHECK_EQUAL(dummy::instances_count, 2);
        }
        BOOST_CHECK_EQUAL(dummy::instances_count, 0);
    }

    BOOST_AUTO_TEST_CASE(does_not_leak_being_move_assigned)
    {
        BOOST_REQUIRE_EQUAL(dummy::instances_count, 0);
        {
            burst::dynamic_tuple t(dummy{});
            BOOST_REQUIRE_EQUAL(dummy::instances_count, 1);

            burst::dynamic_tuple copy(dummy{});
            BOOST_REQUIRE_EQUAL(dummy::instances_count, 2);

            copy = std::move(t);
            BOOST_CHECK_EQUAL(dummy::instances_count, 2);
        }
        BOOST_CHECK_EQUAL(dummy::instances_count, 0);
    }

    BOOST_AUTO_TEST_CASE(can_give_const_element_by_offset)
    {
        const burst::dynamic_tuple t(std::string("123"), 42, true);
        auto int_offset = t.offset(1);
        BOOST_CHECK_EQUAL(t.get_by_offset<int>(int_offset), 42);
    }

    BOOST_AUTO_TEST_CASE(can_give_mutable_element_by_offset)
    {
        auto old_value = 42;
        auto new_value = 99;

        burst::dynamic_tuple t(std::string("123"), old_value, true);
        auto int_offset = t.offset(1);

        BOOST_REQUIRE_EQUAL(t.get<int>(1), old_value);
        t.get_by_offset<int>(int_offset) = new_value;

        BOOST_CHECK_EQUAL(t.get<int>(1), new_value);
    }

    BOOST_AUTO_TEST_CASE(pop_back_decrements_tuple_size)
    {
        auto t = burst::dynamic_tuple(true, 3.14, 42);

        t.pop_back();
        BOOST_CHECK_EQUAL(t.size(), 2);
    }

    BOOST_AUTO_TEST_CASE(volume_after_pop_back_is_equal_to_volume_of_tuple_without_last_element)
    {
        auto t = burst::dynamic_tuple(true, 42);
        t.pop_back();

        BOOST_CHECK_EQUAL(t.volume(), burst::dynamic_tuple(true).volume());
    }

    BOOST_AUTO_TEST_CASE(popped_element_is_destructed)
    {
        auto t = burst::dynamic_tuple(std::string("qwerty"), 42, dummy{});
        BOOST_REQUIRE_EQUAL(dummy::instances_count, 1);

        t.pop_back();
        BOOST_CHECK_EQUAL(dummy::instances_count, 0);
    }

    BOOST_AUTO_TEST_CASE(push_back_increases_size_by_one)
    {
        auto t = burst::dynamic_tuple{};

        t.push_back(1);
        BOOST_CHECK_EQUAL(t.size(), 1);

        t.push_back(3.14);
        BOOST_CHECK_EQUAL(t.size(), 2);
    }

    BOOST_AUTO_TEST_CASE(push_back_increases_volume_to_size_of_respective_struct)
    {
        auto t = burst::dynamic_tuple{};

        t.push_back(1);
        t.push_back(3.14);
        t.push_back(true);
        t.push_back(std::string("qwe"));

        using respective = struct{int a; double b; bool c; std::string d;};
        BOOST_CHECK_EQUAL(t.volume(), sizeof(respective));
    }

    BOOST_AUTO_TEST_CASE(capacity_does_not_change_after_pop_back)
    {
        burst::dynamic_tuple t;
        for (auto i = 0; i < 100; ++i)
        {
            t.push_back(3.14);
        }

        auto old_capacity = t.capacity();

        while (not t.empty())
        {
            t.pop_back();
        }
        BOOST_CHECK_EQUAL(t.capacity(), old_capacity);
    }

    BOOST_AUTO_TEST_CASE(can_give_type_info_by_index)
    {
        const auto t = burst::dynamic_tuple(std::string("123"));
        BOOST_CHECK(t.type(0) == typeid(std::string));
    }

    BOOST_AUTO_TEST_CASE(shrink_to_fit_reduces_capacity_to_volume)
    {
        auto t = burst::dynamic_tuple(std::string("123"), 2.71, true);

        t.reserve(t.volume() * 2);
        t.shrink_to_fit();

        BOOST_CHECK_EQUAL(t.capacity(), t.volume());
    }

    BOOST_AUTO_TEST_CASE(empty_tuple_shrink_to_fit_reduces_capacity_to_zero)
    {
        auto t = burst::dynamic_tuple{};

        t.reserve(100);
        t.shrink_to_fit();

        BOOST_CHECK_EQUAL(t.capacity(), 0);
    }

    BOOST_AUTO_TEST_CASE(shrinked_to_fit_empty_tuple_remains_empty)
    {
        auto t = burst::dynamic_tuple{};

        t.reserve(100);
        t.shrink_to_fit();

        BOOST_CHECK(t.empty());
    }

    BOOST_AUTO_TEST_CASE(shrinked_to_fit_non_empty_tuple_remains_non_empty)
    {
        auto t = burst::dynamic_tuple{1, false};

        t.reserve(t.volume() * 2);
        t.shrink_to_fit();

        BOOST_CHECK(not t.empty());
    }

    BOOST_AUTO_TEST_CASE(shrink_to_fit_does_not_affect_empty_tuple_size)
    {
        auto t = burst::dynamic_tuple{};

        t.reserve(100);
        t.shrink_to_fit();

        BOOST_CHECK_EQUAL(t.size(), 0);
    }

    BOOST_AUTO_TEST_CASE(shrink_to_fit_does_not_affect_non_empty_tuple_size)
    {
        auto t = burst::dynamic_tuple(1, true, 3.14);

        t.reserve(t.volume() * 2);
        t.shrink_to_fit();

        BOOST_CHECK_EQUAL(t.size(), 3);
    }

    BOOST_AUTO_TEST_CASE(shrink_to_fit_does_not_affect_empty_tuple_volume)
    {
        auto t = burst::dynamic_tuple{};

        t.reserve(10);
        t.shrink_to_fit();

        BOOST_CHECK_EQUAL(t.volume(), 0);
    }

    BOOST_AUTO_TEST_CASE(shrink_to_fit_does_not_affect_non_empty_tuple_volume)
    {
        auto t = burst::dynamic_tuple(1, true, 3.14);
        auto old_volume = t.volume();

        t.reserve(t.volume() * 2);
        t.shrink_to_fit();

        BOOST_CHECK_EQUAL(t.volume(), old_volume);
    }

    BOOST_AUTO_TEST_CASE(has_free_swap_function)
    {
        auto t = burst::dynamic_tuple(1, 3.14);
        auto u = burst::dynamic_tuple(std::string("qwe"), true);

        swap(t, u);
    }
BOOST_AUTO_TEST_SUITE_END()
