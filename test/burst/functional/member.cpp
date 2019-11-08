#include <burst/functional/member.hpp>

#include <doctest/doctest.h>

#include <type_traits>

namespace // anonymous
{
    struct dummy
    {
        int member;
    };
} // namespace anonymous

TEST_SUITE("member_macro")
{
    TEST_CASE("Создаёт функциональный объект, вызывающий член класса с заданным именем")
    {
        auto get_member = BURST_MEMBER(member);
        CHECK(get_member(dummy{17}) == 17);
    }

    TEST_CASE("Компонуем")
    {
        const auto square_of_member = BURST_MEMBER(member) | [] (auto x) {return x * x;};
        const auto d = dummy{5};
        CHECK(square_of_member(d) == 25);
    }

    TEST_CASE("Возвращает ссылку на константу, если объект константный")
    {
        const auto member_of = BURST_MEMBER(member);
        const auto d = dummy{42};

        decltype(auto) m = member_of(d);

        CHECK(std::is_same<decltype(m), const int &>::value);
    }

    TEST_CASE("Возвращает ссылку на изменяемое значение, если объект изменяем")
    {
        const auto member_of = BURST_MEMBER(member);
        auto d = dummy{42};

        member_of(d) = 17;

        CHECK(d.member == 17);
    }

    TEST_CASE("Возвращает rvalue, если объект временный")
    {
        const auto member_of = BURST_MEMBER(member);
        CHECK(std::is_same<decltype(member_of(dummy{5})), int &&>::value);
    }
}
