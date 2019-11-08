#include <burst/tuple/apply.hpp>
#include <utility/caller_dummies.hpp>

#include <doctest/doctest.h>

#include <cstddef>
#include <functional>
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

TEST_SUITE("apply")
{
    TEST_CASE("Применяет функцию к элементам развёрнутого кортежа")
    {
        auto a = burst::apply(std::plus<>{});
        auto r = a(std::make_tuple(1, 2));
        CHECK(r == 3);
    }

    TEST_CASE("Переданная функция хранится внутри созданного функционального объекта")
    {
        const auto old_instances_count = dummy::instances_count;

        auto a = burst::apply(dummy{});
        static_cast<void>(a);

        CHECK(dummy::instances_count == old_instances_count + 1);
    }

    TEST_CASE("Переданная функция не хранится внутри, если она передана с помощью std::ref")
    {
        auto d = dummy{};
        const auto old_instances_count = dummy::instances_count;

        auto a = burst::apply(std::ref(d));
        static_cast<void>(a);

        CHECK(dummy::instances_count == old_instances_count);
    }

    TEST_CASE("Если созданный функциональный объект вызывается как const lvalue, то хранимый "
        "функциональный объект вызывается так же")
    {
        auto calls = std::size_t{0};
        const auto a = burst::apply(utility::const_lvalue_call_counter(calls));

        a(std::make_tuple(1, "qwe"));

        CHECK(calls == 1);
    }

    TEST_CASE("Если созданный функциональный объект вызывается как lvalue, то хранимый "
        "функциональный объект вызывается так же")
    {
        auto calls = std::size_t{0};
        auto a = burst::apply(utility::lvalue_call_counter(calls));

        a(std::make_tuple(1, "qwe"));

        CHECK(calls == 1);
    }

    TEST_CASE("Если созданный функциональный объект вызывается как rvalue, то хранимый "
        "функциональный объект вызывается так же")
    {
        auto calls = std::size_t{0};
        auto c = utility::rvalue_call_counter(calls);

        burst::apply(c)(std::make_tuple(1, "qwe"));

        CHECK(calls == 1);
    }

    TEST_CASE("Функция, переданная с помощью std::ref, всегда вызывается как lvalue")
    {
        auto calls = std::size_t{0};
        auto f = utility::lvalue_call_counter(calls);

        burst::apply(std::ref(f))(std::make_tuple(1, "qwe"));

        CHECK(calls == 1);
    }

    TEST_CASE("Неизменяемая функция, переданная с помощью std::ref, всегда вызывается как "
        "const lvalue")
    {
        auto calls = std::size_t{0};
        const auto f = utility::const_lvalue_call_counter(calls);

        burst::apply(std::ref(f))(std::make_tuple(1, "qwe"));

        CHECK(calls == 1);
    }

    TEST_CASE("Может быть вычислен на этапе компиляции")
    {
        constexpr auto a = burst::apply(std::multiplies<>{});
        constexpr auto r = a(std::make_tuple(4, 5));
        CHECK(r == 20);
    }
}
