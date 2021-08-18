#ifndef BURST__FUNCTIONAL__MEM_FN_HPP
#define BURST__FUNCTIONAL__MEM_FN_HPP

#include <burst/functional/func.hpp>

#include <utility>

#define BURST_MEM_FN(f)\
    burst::make_func([] (auto && x, auto && ... args)\
        -> decltype(auto)\
    {\
        return std::forward<decltype(x)>(x).f(std::forward<decltype(args)>(args)...);\
    })

#endif // BURST__FUNCTIONAL__MEM_FN_HPP
