#ifndef BURST__TYPE_TRAITS__ITERATOR_VALUE_HPP
#define BURST__TYPE_TRAITS__ITERATOR_VALUE_HPP

#include <iterator>

namespace burst
{
    template <typename Iterator>
    using iterator_value_t = typename std::iterator_traits<Iterator>::value_type;
}

#endif // BURST__TYPE_TRAITS__ITERATOR_VALUE_HPP
