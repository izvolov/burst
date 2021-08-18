#ifndef BURST__FUNCTIONAL__NON_STRICT_HPP
#define BURST__FUNCTIONAL__NON_STRICT_HPP

#include <burst/functional/invert.hpp>
#include <burst/functional/not_fn.hpp>

#include <utility>

namespace burst
{
    //!     Преобразование строгого порядка в нестрогий
    /*!
            Например, оператор "меньше" превращает в "меньше или равно".
     */
    template <typename BinaryPredicate>
    auto non_strict (BinaryPredicate && strict_order)
    {
        return burst::not_fn(invert(std::forward<BinaryPredicate>(strict_order)));
    }
}

#endif // BURST__FUNCTIONAL__NON_STRICT_HPP
