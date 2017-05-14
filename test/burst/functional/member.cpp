#include <burst/functional/member.hpp>

#include <boost/test/unit_test.hpp>

#include <type_traits>

BOOST_AUTO_TEST_SUITE(member_macro)
    struct dummy
    {
        int member;
    };

    BOOST_AUTO_TEST_CASE(generates_a_function_object_returning_member_of_a_class)
    {
        auto get_member = BURST_MEMBER(member);
        BOOST_CHECK_EQUAL(get_member(dummy{17}), 17);
    }

    BOOST_AUTO_TEST_CASE(is_composable)
    {
        const auto square_of_member = BURST_MEMBER(member) | [] (auto x) {return x * x;};
        const auto d = dummy{5};
        BOOST_CHECK_EQUAL(square_of_member(d), 25);
    }

    BOOST_AUTO_TEST_CASE(returns_a_const_reference_if_object_is_const)
    {
        const auto member_of = BURST_MEMBER(member);
        const auto d = dummy{42};

        decltype(auto) m = member_of(d);

        BOOST_CHECK((std::is_same<decltype(m), const int &>::value));
    }

    BOOST_AUTO_TEST_CASE(returns_a_mutable_reference_if_object_is_mutable)
    {
        const auto member_of = BURST_MEMBER(member);
        auto d = dummy{42};

        member_of(d) = 17;

        BOOST_CHECK_EQUAL(d.member, 17);
    }

    BOOST_AUTO_TEST_CASE(returns_rvalue_if_object_is_rvalue)
    {
        const auto member_of = BURST_MEMBER(member);
        BOOST_CHECK((std::is_same<decltype(member_of(dummy{5})), int &&>::value));
    }
BOOST_AUTO_TEST_SUITE_END()
