#include <burst/functional/identity.hpp>
#include <burst/integer/intlog2.hpp>
#include <burst/tuple/by_all.hpp>
#include <utility/caller_dummies.hpp>
#include <utility/io/tuple.hpp>

#include <doctest/doctest.h>

#include <cstddef>
#include <functional>
#include <tuple>

namespace // anonymous
{
    enum struct initialization_way
    {
        construction,
        copy,
        move
    };

    struct movable
    {
        movable ():
            initialized_by{initialization_way::construction}
        {
        }

        movable (const movable &):
            initialized_by{initialization_way::copy}
        {
        }

        movable (movable &&):
            initialized_by{initialization_way::move}
        {
        }

        ~movable ()
        {
        }

        initialization_way initialized_by;
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
} // namespace anonymous

TEST_SUITE("by_all")
{
    TEST_CASE("Преобразует все элементы кортежа")
    {
        const auto t =
            burst::by_all([] (auto x) {return x * x;}, std::make_tuple(5, 6));
        CHECK(t == std::make_tuple(25, 36));
    }

    TEST_CASE("Принимая только функцию создаёт функциональный объект, который уже может быть "
        "применён непосредственно к кортежу (см. лифт)")
    {
        const auto f = burst::by_all([] (auto x) {return x * x;});
        const auto t = f(std::make_tuple(5, 7));
        CHECK(t == std::make_tuple(25, 49));
    }

    TEST_CASE("Элементы временного исходного кортежа переносятся в результирующий")
    {
        movable m;

        std::tuple<movable, movable> r =
            burst::by_all(burst::identity, std::make_tuple(m, m));

        CHECK(std::get<0>(r).initialized_by == initialization_way::move);
        CHECK(std::get<1>(r).initialized_by == initialization_way::move);
    }

    TEST_CASE("Ссылки пробрасываются")
    {
        auto t1 = std::make_tuple(5, std::string("qwe"));
        auto t2 = std::make_tuple(std::string("asd"), 2.71);
        auto initial = std::make_tuple(std::ref(t1), std::ref(t2));

        const auto reference_to_first_tuple_element =
            [] (auto & t)
                -> decltype(auto)
            {
                return std::get<0>(t);
            };
        auto resulting = burst::by_all(reference_to_first_tuple_element, initial);
        std::get<1>(resulting) = "fgh";

        CHECK(std::get<0>(t2) == "fgh");
    }

    TEST_CASE("Переданная функция хранится внутри созданного функционального объекта")
    {
        const auto old_instances_count = dummy::instances_count;

        auto b = burst::by_all(dummy{});
        static_cast<void>(b);

        CHECK(dummy::instances_count == old_instances_count + 1);
    }

    TEST_CASE("Переданная функция не хранится внутри, если она передана с помощью std::ref")
    {
        auto d = dummy{};
        const auto old_instances_count = dummy::instances_count;

        auto b = burst::by_all(std::ref(d));
        static_cast<void>(b);

        CHECK(dummy::instances_count == old_instances_count);
    }

    TEST_CASE("Если созданный функциональный объект вызывается как const lvalue, то хранимый "
        "функциональный объект вызывается так же")
    {
        auto calls = std::size_t{0};
        const auto b = burst::by_all(utility::const_lvalue_call_counter(calls));

        b(std::make_tuple(1, "qwe"));

        CHECK(calls == 2);
    }

    TEST_CASE("Если созданный функциональный объект вызывается как lvalue, то хранимый "
        "функциональный объект вызывается так же")
    {
        auto calls = std::size_t{0};
        auto b = burst::by_all(utility::lvalue_call_counter(calls));

        b(std::make_tuple(1, "qwe"));

        CHECK(calls == 2);
    }

    TEST_CASE("Хранимый функциональный объект вызывается как lvalue, если созданный функциональный "
        "объект вызывается как rvalue")
    {
        auto calls = std::size_t{0};
        auto l = utility::lvalue_call_counter(calls);

        burst::by_all(l)(std::make_tuple(1, "qwe"));

        CHECK(calls == 2);
    }

    TEST_CASE("Функция, переданная с помощью std::ref, всегда вызывается как lvalue")
    {
        auto calls = std::size_t{0};
        auto f = utility::lvalue_call_counter(calls);

        burst::by_all(std::ref(f))(std::make_tuple(1, "qwe"));

        CHECK(calls == 2);
    }

    TEST_CASE("Неизменяемая функция, переданная с помощью std::ref, всегда вызывается как "
        "const lvalue")
    {
        auto calls = std::size_t{0};
        const auto f = utility::const_lvalue_call_counter(calls);

        burst::by_all(std::ref(f))(std::make_tuple(1, "qwe"));

        CHECK(calls == 2);
    }

    TEST_CASE("Может быть вычислен на этапе компиляции")
    {
        constexpr auto t = std::make_tuple(1, 256, 1024);
        constexpr auto l = burst::by_all(&burst::intlog2<int>);
        constexpr auto r = l(t);
        static_assert(r == std::make_tuple(0, 8, 10), "");
    }
}
