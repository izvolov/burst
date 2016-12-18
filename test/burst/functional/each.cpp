#include <burst/algorithm/sum.hpp>
#include <burst/functional/each.hpp>
#include <burst/integer/intlog2.hpp>
#include <burst/tuple/make_tuple.hpp>
#include <test/output/tuple.hpp>

#include <boost/test/unit_test.hpp>

#include <string>
#include <tuple>

BOOST_AUTO_TEST_SUITE(each)
    BOOST_AUTO_TEST_CASE(forwards_input_arguments_mapped_by_specified_function)
    {
        auto e = burst::each([] (auto x) {return x + x;}) | burst::make_tuple;
        auto t = e(4, std::string("qwe"));
        BOOST_CHECK_EQUAL(t, std::make_tuple(8, "qweqwe"));
    }

    BOOST_AUTO_TEST_CASE(may_be_composed_with_another_each)
    {
        const auto square = [] (auto x) {return x * x;};
        auto e = burst::each(square) | burst::each(square) | burst::sum;
        auto r = e(1, 2, 3);
        BOOST_CHECK_EQUAL(r, 1 + 16 + 81);
    }

    struct doubler
    {
        doubler (std::size_t & calls):
            calls(calls)
        {
        }

        template <typename T>
        auto operator () (T t) const
        {
            ++calls;
            return t * t;
        }

        std::size_t & calls;
    };

    BOOST_AUTO_TEST_CASE(calls_inner_function_on_every_input_element)
    {
        auto calls = std::size_t{0};
        auto e = burst::each(doubler{calls}) | burst::make_tuple;

        e(4, 'a', 3.14);

        BOOST_CHECK_EQUAL(calls, 3);
    }

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

    BOOST_AUTO_TEST_CASE(every_passed_function_is_stored_inside)
    {
        const auto old_instances_count = dummy::instances_count;

        auto e = burst::each(dummy{}) | burst::each(dummy{});
        static_cast<void>(e);

        BOOST_CHECK_EQUAL(dummy::instances_count, old_instances_count + 2);
    }

    BOOST_AUTO_TEST_CASE(function_is_not_stored_if_passed_by_ref)
    {
        auto d = dummy{};
        const auto old_instances_count = dummy::instances_count;

        auto e = burst::each(std::ref(d));
        static_cast<void>(e);

        BOOST_CHECK_EQUAL(dummy::instances_count, old_instances_count);
    }

    struct const_lvalue_caller
    {
        const_lvalue_caller (std::size_t & calls):
            calls(calls)
        {
        }

        template <typename T>
        auto operator () (T t) const &
        {
            ++calls;
            return t;
        }

        template <typename T>
        auto operator () (T t) & = delete;

        template <typename T>
        auto operator () (T t) && = delete;

        std::size_t & calls;
    };

    BOOST_AUTO_TEST_CASE(stored_function_invokes_as_const_lvalue_if_each_is_const_lvalue)
    {
        auto calls = std::size_t{0};
        const auto each = burst::each(const_lvalue_caller{calls}) | burst::make_tuple;

        each(1, "qwe");

        BOOST_CHECK_EQUAL(calls, 2);
    }

    struct lvalue_caller
    {
        lvalue_caller (std::size_t & calls):
            calls(calls)
        {
        }

        template <typename T>
        auto operator () (T t) const & = delete;

        template <typename T>
        auto operator () (T t) &
        {
            ++calls;
            return t;
        }

        template <typename T>
        auto operator () (T t) && = delete;

        std::size_t & calls;
    };

    BOOST_AUTO_TEST_CASE(stored_function_invokes_as_lvalue_if_each_is_lvalue)
    {
        auto calls = std::size_t{0};
        auto each = burst::each(lvalue_caller{calls}) | burst::make_tuple;

        each(1, "qwe");

        BOOST_CHECK_EQUAL(calls, 2);
    }

    struct rvalue_caller
    {
        rvalue_caller (std::size_t & calls):
            calls(calls)
        {
        }

        template <typename Ts>
        void operator () (Ts ...) const & = delete;

        template <typename Ts>
        void operator () (Ts ...) & = delete;

        template <typename Ts>
        void operator () (Ts ...) &&
        {
            ++calls;
        }

        std::size_t & calls;
    };

    BOOST_AUTO_TEST_CASE(composed_function_invokes_as_rvalue_when_each_is_rvalue)
    {
        auto calls = std::size_t{0};
        (burst::each([] (auto x) {return x + x;}) | rvalue_caller{calls})(1, 2, 3);

        BOOST_CHECK_EQUAL(calls, 1);
    }

    BOOST_AUTO_TEST_CASE(stored_function_invokes_as_lvalue_if_each_is_rvalue)
    {
        auto calls = std::size_t{0};
        (burst::each(lvalue_caller{calls}) | burst::make_tuple)(1, "qwe");

        BOOST_CHECK_EQUAL(calls, 2);
    }

    BOOST_AUTO_TEST_CASE(referenced_function_invokes_as_lvalue)
    {
        auto calls = std::size_t{0};
        auto c = lvalue_caller{calls};
        const auto each = burst::each(std::ref(c)) | burst::make_tuple;

        each(1, "qwe");

        BOOST_CHECK_EQUAL(calls, 2);
    }

    BOOST_AUTO_TEST_CASE(const_referenced_function_invokes_as_const_lvalue)
    {
        auto calls = std::size_t{0};
        const auto c = const_lvalue_caller{calls};
        const auto each = burst::each(std::ref(c)) | burst::make_tuple;

        each(1, "qwe");

        BOOST_CHECK_EQUAL(calls, 2);
    }

    BOOST_AUTO_TEST_CASE(is_a_constexpr_function)
    {
        constexpr auto e = burst::each(&burst::intlog2<int>) | burst::make_tuple;
        constexpr auto t = e(1, 256, 1024);
        BOOST_CHECK_EQUAL(t, std::make_tuple(0, 8, 10));
    }
BOOST_AUTO_TEST_SUITE_END()
