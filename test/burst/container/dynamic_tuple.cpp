#include <burst/container/dynamic_tuple.hpp>
#include <utility/io/vector.hpp>

#include <doctest/doctest.h>

#include <string>
#include <vector>

namespace // anonymous
{
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

        ~dummy () noexcept(false)
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
} // namespace anonymous

TEST_SUITE("dynamic_tuple")
{
    TEST_CASE("dynamic_tuple_can_be_created_from_an_arbitrary_set_of_types")
    {
        burst::dynamic_tuple t(std::string("123"), 42, true, X{17, std::vector<int>{1, 2, 3}});
        CHECK(t.size() == 4);
        CHECK(t.get<std::string>(0) == std::string("123"));
        CHECK(t.get<int>(1) == 42);
        CHECK(t.get<bool>(2) == true);
        CHECK(t.get<X>(3).a == 17);
        CHECK(t.get<X>(3).b == std::vector<int>{1, 2, 3});
    }

    TEST_CASE("newly_created_tuple_with_no_elements_is_empty")
    {
        const auto t = burst::dynamic_tuple{};
        CHECK(t.empty());
    }

    TEST_CASE("size_of_newly_created_tuple_with_no_elements_is_zero")
    {
        const auto t = burst::dynamic_tuple{};
        CHECK(t.size() == 0);
    }

    TEST_CASE("volume_of_newly_created_tuple_with_no_elements_is_zero")
    {
        const auto t = burst::dynamic_tuple{};
        CHECK(t.volume() == 0);
    }

    TEST_CASE("capacity_of_newly_created_tuple_with_no_elements_is_zero")
    {
        const auto t = burst::dynamic_tuple{};
        CHECK(t.capacity() == 0);
    }

    TEST_CASE("newly_created_tuple_with_some_elements_is_not_empty")
    {
        const auto t = burst::dynamic_tuple(42, 3.14);
        CHECK(not t.empty());
    }

    TEST_CASE("size_of_newly_created_tuple_with_some_elements_is_amount_of_those_elements")
    {
        const auto t = burst::dynamic_tuple(std::string("123"), 42, 2.71, false);
        CHECK(t.size() == 4);
    }

    TEST_CASE("volume_of_tuple_constructed_from_some_elements_is_equal_to_volume_of_tuple_with_those_elements_consequently_pushed_back")
    {
        const auto constructed = burst::dynamic_tuple(42, 3.14, std::string("123"), true);

        auto pushed_back = burst::dynamic_tuple{};
        pushed_back.push_back(42);
        pushed_back.push_back(3.14);
        pushed_back.push_back(std::string("123"));
        pushed_back.push_back(true);

        CHECK(constructed.volume() == pushed_back.volume());
    }

    TEST_CASE("capacity_of_newly_created_tuple_with_some_elements_is_greater_than_or_equal_to_volume")
    {
        const auto t = burst::dynamic_tuple(std::vector<std::size_t>{}, 42, X{}, 3.14);
        CHECK(t.capacity() >= t.volume());
    }

    TEST_CASE("reserving_less_space_than_already_present_does_nothing")
    {
        auto t = burst::dynamic_tuple(true, 42, std::string("qwe"));
        auto old_capacity = t.capacity();

        t.reserve(t.capacity() / 2);

        CHECK(t.capacity() == old_capacity);
    }

    TEST_CASE("reserving_space_equal_to_current_capacity_does_nothing")
    {
        auto t = burst::dynamic_tuple(true, 42, std::string("qwe"));
        auto old_capacity = t.capacity();

        t.reserve(t.capacity());

        CHECK(t.capacity() == old_capacity);
    }

    TEST_CASE("new_capacity_of_empty_tuple_is_greater_or_equal_to_value_of_reserved_space")
    {
        auto t = burst::dynamic_tuple{};

        t.reserve(10);

        CHECK(t.capacity() >= 10);
    }

    TEST_CASE("new_capacity_of_non_empty_tuple_is_greater_or_equal_to_value_of_reserved_space")
    {
        auto t = burst::dynamic_tuple(false, 3.14, 42);

        auto desired_capacity = t.capacity() + 10;
        t.reserve(desired_capacity);

        CHECK(t.capacity() >= desired_capacity);
    }

    TEST_CASE("size_of_dynamic_tuple_is_equal_to_count_of_inlying_objects")
    {
        const auto some_vector = std::vector<std::size_t>{};
        const auto some_integer = 42;
        const auto some_struct = X{};
        const auto some_floating = 3.14;

        burst::dynamic_tuple t(some_vector, some_integer, some_struct, some_floating);
        CHECK(t.size() == 4);
    }

    TEST_CASE("arbitrary_objects_can_be_pushed_back")
    {
        burst::dynamic_tuple t;

        t.push_back(X{17, std::vector<int>{1, 2, 3}});
        t.push_back(true);
        t.push_back(std::string("123"));

        CHECK(t.size() == 3);
        CHECK(t.get<X>(0).a == 17);
        CHECK(t.get<X>(0).b == std::vector<int>{1, 2, 3});
        CHECK(t.get<bool>(1) == true);
        CHECK(t.get<std::string>(2) == std::string("123"));
    }

    TEST_CASE("clear_method_destructs_inlying_objects")
    {
        burst::dynamic_tuple t;
        t.push_back(dummy{});

        REQUIRE(dummy::instances_count == 1);
        t.clear();

        CHECK(dummy::instances_count == 0);
    }

    TEST_CASE("volume_is_zero_after_clear")
    {
        burst::dynamic_tuple t(1, 3.14, true, std::string("qwe"));

        t.clear();

        CHECK(t.volume() == 0);
    }

    TEST_CASE("size_is_zero_after_clear")
    {
        burst::dynamic_tuple t(1, 3.14, true, std::string("qwe"));

        t.clear();

        CHECK(t.size() == 0);
    }

    TEST_CASE("dynamic_tuple_is_empty_after_clear")
    {
        burst::dynamic_tuple t(1, 3.14, true, std::string("qwe"));

        t.clear();

        CHECK(t.empty());
    }

    TEST_CASE("capacity_does_not_change_after_clear")
    {
        burst::dynamic_tuple t(1, 3.14, true, std::string("qwe"));
        const auto capacity = t.capacity();

        t.clear();

        CHECK(t.capacity() == capacity);
    }

    TEST_CASE("does_not_leak_when_inserted_element_throws")
    {
        REQUIRE_THROWS_AS(burst::dynamic_tuple(kamikaze{}, kamikaze{}, kamikaze{}), std::runtime_error);
        CHECK(kamikaze::instances_count == 0);
    }

    TEST_CASE("underlying_items_can_be_mutated")
    {
        burst::dynamic_tuple t(true, std::vector<char>{'a', 'b' ,'c'}, 10);

        auto & vector = t.get<std::vector<char>>(1);

        vector.push_back('q');
        CHECK(t.get<std::vector<char>>(1) == std::vector<char>{'a', 'b' ,'c', 'q'});

        vector.clear();
        CHECK(t.get<std::vector<char>>(1).empty());
    }

    TEST_CASE("moved_dynamic_tuple_is_actually_moved_and_is_not_emplaced_back")
    {
        burst::dynamic_tuple moved(2.71, 3, false);
        burst::dynamic_tuple constructed(std::move(moved));
        CHECK(constructed.size() == 3);
    }

    TEST_CASE("copied_dynamic_tuple_is_actually_copied_and_is_not_emplaced_back")
    {
        burst::dynamic_tuple copied(2.71, 3, false);
        burst::dynamic_tuple constructed(copied);
        CHECK(constructed.size() == 3);
    }

    TEST_CASE("tuple_of_tuples_is_possible")
    {
        burst::dynamic_tuple t(burst::dynamic_tuple{}, burst::dynamic_tuple{});
        CHECK(t.size() == 2);
    }

    TEST_CASE("inlying_objects_move_together_with_dynamic_tuple")
    {
        REQUIRE(dummy::instances_count == 0);
        burst::dynamic_tuple one(dummy{});
        REQUIRE(dummy::instances_count == 1);

        burst::dynamic_tuple another(std::move(one));
        CHECK(dummy::instances_count == 1);

        one = std::move(another);
        CHECK(dummy::instances_count == 1);
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

    TEST_CASE("does_not_leak_when_inlying_element_throws_being_moved")
    {
        const throw_on_move thrower{};

        REQUIRE(dummy::instances_count == 0);
        {
            burst::dynamic_tuple t;
            t.push_back(dummy{});
            t.push_back(thrower);
            REQUIRE(dummy::instances_count == 1);

            REQUIRE_THROWS_AS(t.reserve(t.capacity() + 1), std::runtime_error);
            CHECK(dummy::instances_count == 1);
        }
        CHECK(dummy::instances_count == 0);
    }

    TEST_CASE("proper_ctor_dtor_calls_of_inlying_objects_in_dyntuple_copy_constructor")
    {
        REQUIRE(dummy::instances_count == 0);
        {
            burst::dynamic_tuple initial(dummy{});

            REQUIRE(dummy::instances_count == 1);
            {
                burst::dynamic_tuple copy(initial);
                CHECK(dummy::instances_count == 2);
            }
            CHECK(dummy::instances_count == 1);
        }
        CHECK(dummy::instances_count == 0);
    }

    TEST_CASE("proper_ctor_dtor_calls_of_inlying_objects_in_dyntuple_copy_assignment")
    {
        REQUIRE(dummy::instances_count == 0);
        {
            burst::dynamic_tuple initial(dummy{});

            REQUIRE(dummy::instances_count == 1);
            {
                burst::dynamic_tuple copy(dummy{});
                copy = initial;
                CHECK(dummy::instances_count == 2);
            }
            CHECK(dummy::instances_count == 1);
        }
        CHECK(dummy::instances_count == 0);
    }

    TEST_CASE("inlying_objects_are_destroyed_during_copy_assignment")
    {
        REQUIRE(dummy::instances_count == 0);
        burst::dynamic_tuple initial(dummy{});
        REQUIRE(dummy::instances_count == 1);

        burst::dynamic_tuple other(1, 3.14, true);
        initial = other;
        CHECK(dummy::instances_count == 0);
    }

    TEST_CASE("inlying_objects_are_destroyed_during_move_assignment")
    {
        REQUIRE(dummy::instances_count == 0);
        burst::dynamic_tuple initial(dummy{});
        REQUIRE(dummy::instances_count == 1);

        burst::dynamic_tuple other(1, 3.14, true);
        initial = std::move(other);
        CHECK(dummy::instances_count == 0);
    }

    TEST_CASE("source_is_empty_after_move_construction")
    {
        burst::dynamic_tuple source(13, 3.14, true);
        burst::dynamic_tuple destination(std::move(source));
        CHECK(source.empty());
    }

    TEST_CASE("source_is_empty_after_move_assignment")
    {
        burst::dynamic_tuple source(std::string("move me"));
        burst::dynamic_tuple destination(std::string("some items here"));
        destination = std::move(source);
        CHECK(source.empty());
    }

    TEST_CASE("source_size_is_zero_after_move_construction")
    {
        burst::dynamic_tuple source(13, 3.14, true);
        burst::dynamic_tuple destination(std::move(source));
        CHECK(source.size() == 0);
    }

    TEST_CASE("source_size_is_zero_after_move_assignment")
    {
        burst::dynamic_tuple source(std::string("move me"));
        burst::dynamic_tuple destination(std::string("some items here"));
        destination = std::move(source);
        CHECK(source.size() == 0);
    }

    TEST_CASE("source_volume_is_zero_after_move_construction")
    {
        burst::dynamic_tuple source(13, 3.14, true);
        burst::dynamic_tuple destination(std::move(source));
        CHECK(source.volume() == 0);
    }

    TEST_CASE("source_volume_is_zero_after_move_assignment")
    {
        burst::dynamic_tuple source(std::string("move me"));
        burst::dynamic_tuple destination(std::string("some items here"));
        destination = std::move(source);
        CHECK(source.volume() == 0);
    }

    TEST_CASE("copy_construction_is_deep")
    {
        burst::dynamic_tuple initial(std::string("cat"));

        auto copy = burst::dynamic_tuple(initial);
        REQUIRE(copy.get<std::string>(0) == "cat");

        initial.get<std::string>(0).append("harsis");
        CHECK(copy.get<std::string>(0) == "cat");
    }

    TEST_CASE("copy_assignment_is_deep")
    {
        burst::dynamic_tuple initial(std::vector<std::string>(1, "dog"));

        burst::dynamic_tuple copy(std::string("qwerty"));
        copy = initial;
        REQUIRE(copy.get<std::vector<std::string>>(0) == std::vector<std::string>(1, "dog"));

        initial.get<std::vector<std::string>>(0).push_back("horse");
        CHECK(copy.get<std::vector<std::string>>(0) == std::vector<std::string>(1, "dog"));
    }

    TEST_CASE("volume_is_equal_after_copy_assignment")
    {
        burst::dynamic_tuple initial(std::string("cat"), 5, 3.14, std::vector<int>{1, 2});

        burst::dynamic_tuple copy(true, 2.71);
        copy = initial;
        CHECK(initial.volume() == copy.volume());
    }

    TEST_CASE("sizes_are_equal_after_copy_assignment")
    {
        burst::dynamic_tuple initial(std::string("cat"), 5, 3.14, std::vector<int>{1, 2});

        burst::dynamic_tuple copy(17);
        copy = initial;
        CHECK(initial.size() == copy.size());
    }

    TEST_CASE("copy_assignment_of_empty_tuple_results_empty_tuple")
    {
        burst::dynamic_tuple initial;

        burst::dynamic_tuple copy(std::string("123"), 'a');
        copy = initial;
        CHECK(copy.empty());
    }

    TEST_CASE("throws_on_attempt_to_copy_noncopyable_object")
    {
        burst::dynamic_tuple initial(std::make_unique<int>(5));
        CHECK_THROWS_AS(burst::dynamic_tuple copy(initial), std::runtime_error);
    }

    TEST_CASE("does_not_leak_when_inlying_element_throws_being_copy_constructed")
    {
        REQUIRE(dummy::instances_count == 0);
        {
            burst::dynamic_tuple t(dummy{}, std::make_unique<int>(17));
            REQUIRE(dummy::instances_count == 1);

            REQUIRE_THROWS_AS(burst::dynamic_tuple copy(t), std::runtime_error);
            CHECK(dummy::instances_count == 1);
        }
        CHECK(dummy::instances_count == 0);
    }

    TEST_CASE("does_not_leak_when_inlying_element_throws_being_copy_assigned")
    {
        REQUIRE(dummy::instances_count == 0);
        {
            burst::dynamic_tuple t(dummy{}, std::make_unique<int>(17));
            burst::dynamic_tuple copy(dummy{});
            REQUIRE(dummy::instances_count == 2);

            REQUIRE_THROWS_AS(copy = t, std::runtime_error);
        }
        CHECK(dummy::instances_count == 0);
    }

    TEST_CASE("does_not_leak_being_move_assigned")
    {
        REQUIRE(dummy::instances_count == 0);
        {
            burst::dynamic_tuple t(dummy{});
            REQUIRE(dummy::instances_count == 1);

            burst::dynamic_tuple copy(dummy{});
            REQUIRE(dummy::instances_count == 2);

            copy = std::move(t);
        }
        CHECK(dummy::instances_count == 0);
    }

    TEST_CASE("can_give_const_element_by_offset")
    {
        const burst::dynamic_tuple t(std::string("123"), 42, true);
        auto int_offset = t.offset(1);
        CHECK(t.get_by_offset<int>(int_offset) == 42);
    }

    TEST_CASE("can_give_mutable_element_by_offset")
    {
        auto old_value = 42;
        auto new_value = 99;

        burst::dynamic_tuple t(std::string("123"), old_value, true);
        auto int_offset = t.offset(1);

        REQUIRE(t.get<int>(1) == old_value);
        t.get_by_offset<int>(int_offset) = new_value;

        CHECK(t.get<int>(1) == new_value);
    }

    TEST_CASE("pop_back_decrements_tuple_size")
    {
        auto t = burst::dynamic_tuple(true, 3.14, 42);

        t.pop_back();
        CHECK(t.size() == 2);
    }

    TEST_CASE("volume_after_pop_back_is_equal_to_volume_of_tuple_without_last_element")
    {
        auto t = burst::dynamic_tuple(true, 42);
        t.pop_back();

        CHECK(t.volume() == burst::dynamic_tuple(true).volume());
    }

    TEST_CASE("popped_element_is_destructed")
    {
        auto t = burst::dynamic_tuple(std::string("qwerty"), 42, dummy{});
        REQUIRE(dummy::instances_count == 1);

        t.pop_back();
        CHECK(dummy::instances_count == 0);
    }

    TEST_CASE("push_back_increases_size_by_one")
    {
        auto t = burst::dynamic_tuple{};

        t.push_back(1);
        CHECK(t.size() == 1);

        t.push_back(3.14);
        CHECK(t.size() == 2);
    }

    TEST_CASE("push_back_increases_volume_to_alignment_of_the_inserted_element_plus_its_size")
    {
        auto t = burst::dynamic_tuple{};

        using aligned_by_4 = struct alignas(4) {char a[4];};
        using aligned_by_8 = struct alignas(8) {char a[8];};

        t.push_back(aligned_by_4{});
        CHECK(t.volume() == 4);

        // Вставка элемента, выравнивание которого больше размера предыдущего элемента
        {
            t.push_back(aligned_by_8{});
            CHECK(t.volume() == 16);
        }

        // Вставка элемента, размер которого меньше текущего выравнивания
        {
            t.push_back('a');
            CHECK(t.volume() == 17);
        }
    }

    TEST_CASE("capacity_does_not_change_after_pop_back")
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
        CHECK(t.capacity() == old_capacity);
    }

    TEST_CASE("can_give_type_info_by_index")
    {
        const auto t = burst::dynamic_tuple(std::string("123"));
        CHECK(t.type(0) == typeid(std::string));
    }

    TEST_CASE("shrink_to_fit_reduces_capacity_to_volume")
    {
        auto t = burst::dynamic_tuple(std::string("123"), 2.71, true);

        t.reserve(t.volume() * 2);
        t.shrink_to_fit();

        CHECK(t.capacity() == t.volume());
    }

    TEST_CASE("empty_tuple_shrink_to_fit_reduces_capacity_to_zero")
    {
        auto t = burst::dynamic_tuple{};

        t.reserve(100);
        t.shrink_to_fit();

        CHECK(t.capacity() == 0);
    }

    TEST_CASE("shrinked_to_fit_empty_tuple_remains_empty")
    {
        auto t = burst::dynamic_tuple{};

        t.reserve(100);
        t.shrink_to_fit();

        CHECK(t.empty());
    }

    TEST_CASE("shrinked_to_fit_non_empty_tuple_remains_non_empty")
    {
        auto t = burst::dynamic_tuple{1, false};

        t.reserve(t.volume() * 2);
        t.shrink_to_fit();

        CHECK(not t.empty());
    }

    TEST_CASE("shrink_to_fit_does_not_affect_empty_tuple_size")
    {
        auto t = burst::dynamic_tuple{};

        t.reserve(100);
        t.shrink_to_fit();

        CHECK(t.size() == 0);
    }

    TEST_CASE("shrink_to_fit_does_not_affect_non_empty_tuple_size")
    {
        auto t = burst::dynamic_tuple(1, true, 3.14);

        t.reserve(t.volume() * 2);
        t.shrink_to_fit();

        CHECK(t.size() == 3);
    }

    TEST_CASE("shrink_to_fit_does_not_affect_empty_tuple_volume")
    {
        auto t = burst::dynamic_tuple{};

        t.reserve(10);
        t.shrink_to_fit();

        CHECK(t.volume() == 0);
    }

    TEST_CASE("shrink_to_fit_does_not_affect_non_empty_tuple_volume")
    {
        auto t = burst::dynamic_tuple(1, true, 3.14);
        auto old_volume = t.volume();

        t.reserve(t.volume() * 2);
        t.shrink_to_fit();

        CHECK(t.volume() == old_volume);
    }

    TEST_CASE("has_free_swap_function")
    {
        auto t = burst::dynamic_tuple(1, 3.14);
        auto u = burst::dynamic_tuple(std::string("qwe"), true);

        swap(t, u);
    }
}
