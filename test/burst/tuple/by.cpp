#include <burst/functional/identity.hpp>
#include <burst/integer/intlog2.hpp>
#include <burst/tuple/by.hpp>
#include <utility/caller_dummies.hpp>

#include <doctest/doctest.h>

#include <cstddef>
#include <functional>
#include <ostream>
#include <string>
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

TEST_SUITE("by")
{
    TEST_CASE("Преобразует заданный элемент кортежа")
    {
        const auto t =
            burst::by<0>([] (auto x) {return x * x;}, std::make_tuple(5, 6));
        CHECK(std::get<0>(t) == 25);
    }

    TEST_CASE("Принимая только функцию создаёт функциональный объект, который уже может быть "
        "применён непосредственно к кортежу (см. лифт)")
    {
        const auto f = burst::by<0>([] (auto x) {return x * x;});
        const auto t = f(std::make_tuple(5, 2.71));
        CHECK(std::get<0>(t) == 25);
    }

    TEST_CASE("Никакие элементы, кроме заданного, не изменяются")
    {
        const auto t = std::make_tuple(5, 2.71, std::string("qwe"));
        const auto f = burst::by<1>([] (auto x) {return x * x;}, t);

        CHECK(std::get<0>(f) == 5);
        CHECK(std::get<2>(f) == "qwe");
    }

    TEST_CASE("Результирующий кортеж никак не связан с исходным")
    {
        auto initial = std::make_tuple(42, std::string("qwe"), 3.14);

        auto resulting = burst::by<0>([] (auto x) {return x + 2;}, initial);
        std::get<1>(resulting) = "asd";

        CHECK(std::get<1>(initial) == "qwe");
    }

    TEST_CASE("Элементы временного исходного кортежа переносятся в результирующий")
    {
        movable m;

        std::tuple<movable, movable> r =
            burst::by<0>(burst::identity, std::make_tuple(m, m));

        CHECK(std::get<0>(r).initialized_by == initialization_way::move);
        CHECK(std::get<1>(r).initialized_by == initialization_way::move);
    }

    TEST_CASE("Нетронутые ссылки остаются ссылками и в результирующем кортеже")
    {
        auto x = 5;
        auto y = 7;
        auto initial = std::make_tuple(std::ref(x), 123, std::cref(y));

        auto resulting = burst::by<1>([] (auto x) {return std::to_string(x);}, initial);
        std::get<0>(resulting) = 555;

        CHECK(x == 555);

        y = 777;
        CHECK(std::get<2>(resulting) == 777);
    }

    TEST_CASE("Ссылки пробрасываются")
    {
        auto t = std::make_tuple(5, std::string("qwe"));
        auto initial = std::make_tuple(1, 3.14, std::ref(t), true);

        auto reference_to_first_tuple_element =
            [] (auto & t)
                -> decltype(auto)
            {
                return std::get<0>(t);
            };
        auto resulting = burst::by<2>(reference_to_first_tuple_element, initial);
        std::get<2>(resulting) = 555;

        CHECK(std::get<0>(t) == 555);
    }

    TEST_CASE("Переданная функция хранится внутри созданного функционального объекта")
    {
        const auto old_instances_count = dummy::instances_count;

        auto b = burst::by<0>(dummy{});
        static_cast<void>(b);

        CHECK(dummy::instances_count == old_instances_count + 1);
    }

    TEST_CASE("Переданная функция не хранится внутри, если она передана с помощью std::ref")
    {
        auto d = dummy{};
        const auto old_instances_count = dummy::instances_count;

        auto b = burst::by<0>(std::ref(d));
        static_cast<void>(b);

        CHECK(dummy::instances_count == old_instances_count);
    }

    TEST_CASE("Если созданный функциональный объект вызывается как const lvalue, то хранимый "
        "функциональный объект вызывается так же")
    {
        auto calls = std::size_t{0};
        const auto by = burst::by<0>(utility::const_lvalue_call_counter(calls));

        by(std::make_tuple(1, "qwe"));

        CHECK(calls == 1);
    }

    TEST_CASE("Если созданный функциональный объект вызывается как lvalue, то хранимый "
        "функциональный объект вызывается так же")
    {
        auto calls = std::size_t{0};
        auto by = burst::by<0>(utility::lvalue_call_counter(calls));

        by(std::make_tuple(1, "qwe"));

        CHECK(calls == 1);
    }

    TEST_CASE("Если созданный функциональный объект вызывается как rvalue, то хранимый "
        "функциональный объект вызывается так же")
    {
        auto calls = std::size_t{0};
        auto c = utility::rvalue_call_counter(calls);

        burst::by<0>(c)(std::make_tuple(1, "qwe"));

        CHECK(calls == 1);
    }

    TEST_CASE("Функция, переданная с помощью std::ref, всегда вызывается как lvalue")
    {
        auto calls = std::size_t{0};
        auto f = utility::lvalue_call_counter(calls);

        burst::by<0>(std::ref(f))(std::make_tuple(1, "qwe"));

        CHECK(calls == 1);
    }

    TEST_CASE("Неизменяемая функция, переданная с помощью std::ref, всегда вызывается как "
        "const lvalue")
    {
        auto calls = std::size_t{0};
        const auto f = utility::const_lvalue_call_counter(calls);

        burst::by<0>(std::ref(f))(std::make_tuple(1, "qwe"));

        CHECK(calls == 1);
    }

    TEST_CASE("Может быть вычислен на этапе компиляции")
    {
        constexpr auto t = std::make_tuple(0, 256, 1024);
        constexpr auto l = burst::by<1>(&burst::intlog2<int>);
        constexpr auto r = l(t);
        CHECK(std::get<1>(r) == 8);
    }
}
