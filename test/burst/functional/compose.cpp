#include <burst/functional/compose.hpp>
#include <burst/tuple/apply.hpp>
#include <burst/tuple/view.hpp>

#include <boost/test/unit_test.hpp>

#include <cstddef>
#include <functional>
#include <string>
#include <tuple>

BOOST_AUTO_TEST_SUITE(compose)
    BOOST_AUTO_TEST_CASE(creates_a_composition_of_two_functions)
    {
        auto c = burst::compose(burst::apply(std::plus<>{}), burst::view<0, 1>);
        auto r = c(std::make_tuple(1, 2));
        BOOST_CHECK_EQUAL(r, 3);
    }

    BOOST_AUTO_TEST_CASE(has_a_pipe_alias)
    {
        auto c = burst::view<0, 1> | burst::apply(std::plus<>{});
        auto r = c(std::make_tuple(1, 2));
        BOOST_CHECK_EQUAL(r, 3);
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

    BOOST_AUTO_TEST_CASE(passed_left_function_is_stored_inside)
    {
        const auto old_instances_count = dummy::instances_count;

        auto c = burst::compose(dummy{}, [] (auto) {return true;});
        static_cast<void>(c);

        BOOST_CHECK_EQUAL(dummy::instances_count, old_instances_count + 1);
    }

    BOOST_AUTO_TEST_CASE(passed_right_function_is_stored_inside)
    {
        const auto old_instances_count = dummy::instances_count;

        auto c = burst::compose([] (auto) {return true;}, dummy{});
        static_cast<void>(c);

        BOOST_CHECK_EQUAL(dummy::instances_count, old_instances_count + 1);
    }

    BOOST_AUTO_TEST_CASE(functions_are_not_stored_if_passed_by_ref)
    {
        auto d = dummy{};
        const auto old_instances_count = dummy::instances_count;

        auto c = burst::compose(std::ref(d), std::cref(d));
        static_cast<void>(c);

        BOOST_CHECK_EQUAL(dummy::instances_count, old_instances_count);
    }

    struct const_lvalue_sum
    {
        const_lvalue_sum (std::size_t & calls):
            calls(calls)
        {
        }

        template <typename T, typename U>
        auto operator () (T t, U u) const &
        {
            ++calls;
            return t + u;
        }

        template <typename T, typename U>
        auto operator () (T t, U u) & = delete;

        template <typename T, typename U>
        auto operator () (T t, U u) && = delete;

        std::size_t & calls;
    };

    struct const_lvalue_identity
    {
        const_lvalue_identity (std::size_t & calls):
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

    BOOST_AUTO_TEST_CASE(stored_functions_invoke_as_const_lvalues_when_compose_is_const_lvalue)
    {
        auto sum_calls = std::size_t{0};
        auto identity_calls = std::size_t{0};
        const auto c =
            burst::compose(const_lvalue_identity{identity_calls}, const_lvalue_sum{sum_calls});

        c(std::string("qwe"), std::string("rty"));

        BOOST_CHECK_EQUAL(sum_calls, 1);
        BOOST_CHECK_EQUAL(identity_calls, 1);
    }

    struct lvalue_sum
    {
        lvalue_sum (std::size_t & calls):
            calls(calls)
        {
        }

        template <typename T, typename U>
        auto operator () (T t, U u) const & = delete;

        template <typename T, typename U>
        auto operator () (T t, U u) &
        {
            ++calls;
            return t + u;
        }

        template <typename T, typename U>
        auto operator () (T t, U u) && = delete;

        std::size_t & calls;
    };

    struct lvalue_identity
    {
        lvalue_identity (std::size_t & calls):
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

    BOOST_AUTO_TEST_CASE(stored_functions_invoke_as_lvalues_when_compose_is_lvalue)
    {
        auto sum_calls = std::size_t{0};
        auto identity_calls = std::size_t{0};
        auto c = burst::compose(lvalue_identity{identity_calls}, lvalue_sum{sum_calls});

        c(std::string("qwe"), std::string("rty"));

        BOOST_CHECK_EQUAL(sum_calls, 1);
        BOOST_CHECK_EQUAL(identity_calls, 1);
    }

    struct rvalue_sum
    {
        rvalue_sum (std::size_t & calls):
            calls(calls)
        {
        }

        template <typename T, typename U>
        auto operator () (T t, U u) const & = delete;

        template <typename T, typename U>
        auto operator () (T t, U u) & = delete;

        template <typename T, typename U>
        auto operator () (T t, U u) &&
        {
            ++calls;
            return t + u;
        }

        std::size_t & calls;
    };

    struct rvalue_identity
    {
        rvalue_identity (std::size_t & calls):
            calls(calls)
        {
        }

        template <typename T>
        auto operator () (T t) const & = delete;

        template <typename T>
        auto operator () (T t) & = delete;

        template <typename T>
        auto operator () (T t) &&
        {
            ++calls;
            return t;
        }

        std::size_t & calls;
    };

    BOOST_AUTO_TEST_CASE(stored_functions_invoke_as_rvalues_when_compose_is_rvalue)
    {
        auto sum_calls = std::size_t{0};
        auto identity_calls = std::size_t{0};

        burst::compose(rvalue_identity{identity_calls}, rvalue_sum{sum_calls})
        (
            std::string("qwe"),
            std::string("rty")
        );

        BOOST_CHECK_EQUAL(sum_calls, 1);
        BOOST_CHECK_EQUAL(identity_calls, 1);
    }

    BOOST_AUTO_TEST_CASE(referenced_functions_invoke_as_lvalues)
    {
        auto identity_calls = std::size_t{0};
        auto identity = lvalue_identity{identity_calls};

        burst::compose(std::ref(identity), [] (auto x, auto y) {return x + y;})
        (
            std::string("qwe"),
            std::string("rty")
        );

        BOOST_CHECK_EQUAL(identity_calls, 1);
    }

    BOOST_AUTO_TEST_CASE(const_referenced_functions_invoke_as_const_lvalues)
    {
        auto sum_calls = std::size_t{0};
        const auto sum = const_lvalue_sum{sum_calls};

        burst::compose([] (auto x) {return x;}, std::ref(sum))
        (
            std::string("qwe"),
            std::string("rty")
        );

        BOOST_CHECK_EQUAL(sum_calls, 1);
    }
BOOST_AUTO_TEST_SUITE_END()
