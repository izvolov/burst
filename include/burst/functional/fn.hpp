#ifndef BURST__FUNCTIONAL__FN_HPP
#define BURST__FUNCTIONAL__FN_HPP

#include <burst/functional/func.hpp>

#include <utility>

#define BURST_FN(f)\
    burst::make_func([] (auto && ... args)\
        -> decltype(auto)\
    {\
        return f(std::forward<decltype(args)>(args)...);\
    })

#endif // BURST__FUNCTIONAL__FN_HPP
