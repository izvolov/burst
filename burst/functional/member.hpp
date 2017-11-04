#ifndef BURST_FUNCTIONAL_MEMBER_HPP
#define BURST_FUNCTIONAL_MEMBER_HPP

#include <burst/functional/func.hpp>

#include <utility>

#define BURST_MEMBER(m)\
    burst::make_func([] (auto && x)\
        -> decltype(auto)\
    {\
        /* Тот самый редкий случай, когда скобки вокруг возвращаемого выражения */\
        /* действительно нужны */\
        return (std::forward<decltype(x)>(x).m);\
    })

#endif // BURST_FUNCTIONAL_MEMBER_HPP
