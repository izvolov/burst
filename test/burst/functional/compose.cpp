#include <burst/functional/compose.hpp>
#include <burst/tuple/apply.hpp>
#include <burst/tuple/view.hpp>
#include <utility/caller_dummies.hpp>

#include <doctest/doctest.h>

#include <cstddef>
#include <functional>
#include <string>
#include <tuple>

namespace // anonymous
{
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

TEST_SUITE("compose")
{
    TEST_CASE("Создаёт композицию двух функций")
    {
        auto c = burst::compose(burst::apply(std::plus<>{}), burst::view<0, 1>);
        auto r = c(std::make_tuple(1, 2));
        CHECK(r == 3);
    }

    TEST_CASE("Имеет псевдоним в виде оператора конвейера (вертикальная палка)")
    {
        auto c = burst::view<0, 1> | burst::apply(std::plus<>{});
        auto r = c(std::make_tuple(1, 2));
        CHECK(r == 3);
    }

    TEST_CASE("Переданная левая функция хранится в созданном функциональном объекте")
    {
        const auto old_instances_count = dummy::instances_count;

        auto c = burst::compose(dummy{}, [] (auto) {return true;});
        static_cast<void>(c);

        CHECK(dummy::instances_count == old_instances_count + 1);
    }

    TEST_CASE("Переданная правая функция хранится в созданном функциональном объекте")
    {
        const auto old_instances_count = dummy::instances_count;

        auto c = burst::compose([] (auto) {return true;}, dummy{});
        static_cast<void>(c);

        CHECK(dummy::instances_count == old_instances_count + 1);
    }

    TEST_CASE("Функции, переданные с помощью std::ref, не хранятся внутри")
    {
        auto d = dummy{};
        const auto old_instances_count = dummy::instances_count;

        auto c = burst::compose(std::ref(d), std::cref(d));
        static_cast<void>(c);

        CHECK(dummy::instances_count == old_instances_count);
    }

    TEST_CASE("Если созданный функциональный объект вызывается как const lvalue, то хранимые "
        "функциональные объекты вызываются так же")
    {
        auto sum_calls = std::size_t{0};
        auto identity_calls = std::size_t{0};
        const auto c =
            burst::compose
            (
                utility::const_lvalue_call_counter(identity_calls, [] (auto x) {return x;}),
                utility::const_lvalue_call_counter(sum_calls, std::plus<>{})
            );

        c(std::string("qwe"), std::string("rty"));

        CHECK(sum_calls == 1);
        CHECK(identity_calls == 1);
    }

    TEST_CASE("Если созданный функциональный объект вызывается как lvalue, то хранимые "
        "функциональные объекты вызываются так же")
    {
        auto sum_calls = std::size_t{0};
        auto identity_calls = std::size_t{0};
        auto c =
            burst::compose
            (
                utility::lvalue_call_counter(identity_calls, [] (auto x) {return x;}),
                utility::lvalue_call_counter(sum_calls, std::plus<>{})
            );

        c(std::string("qwe"), std::string("rty"));

        CHECK(sum_calls == 1);
        CHECK(identity_calls == 1);
    }

    TEST_CASE("Если созданный функциональный объект вызывается как rvalue, то хранимые "
        "функциональные объекты вызываются так же")
    {
        auto sum_calls = std::size_t{0};
        auto identity_calls = std::size_t{0};

        burst::compose
        (
            utility::rvalue_call_counter(identity_calls, [] (auto x) {return x;}),
            utility::rvalue_call_counter(sum_calls, std::plus<>{})
        )
        (
            std::string("qwe"),
            std::string("rty")
        );

        CHECK(sum_calls == 1);
        CHECK(identity_calls == 1);
    }

    TEST_CASE("Функции, переданные с помощью std::ref, всегда вызываются как lvalue")
    {
        auto identity_calls = std::size_t{0};
        auto identity = utility::lvalue_call_counter(identity_calls, [] (auto x) {return x;});

        burst::compose(std::ref(identity), [] (auto x, auto y) {return x + y;})
        (
            std::string("qwe"),
            std::string("rty")
        );

        CHECK(identity_calls == 1);
    }

    TEST_CASE("Неизменяемые функции, переданные с помощью std::ref, всегда вызываются как "
        "const lvalue")
    {
        auto sum_calls = std::size_t{0};
        const auto sum = utility::const_lvalue_call_counter(sum_calls, std::plus<>{});

        burst::compose([] (auto x) {return x;}, std::ref(sum))
        (
            std::string("qwe"),
            std::string("rty")
        );

        CHECK(sum_calls == 1);
    }
}
