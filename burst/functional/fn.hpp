#ifndef BURST_FUNCTIONAL_FN_HPP
#define BURST_FUNCTIONAL_FN_HPP

#include <burst/functional/func.hpp>

#include <utility>

#define BURST_FN(f)\
    burst::make_func([] (auto && ... args)\
        -> decltype(auto)\
    {\
        return f(std::forward<decltype(args)>(args)...);\
    })

#endif // BURST_FUNCTIONAL_FN_HPP
