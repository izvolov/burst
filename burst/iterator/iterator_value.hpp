#ifndef BURST_ITERATOR_ITERATOR_VALUE_HPP
#define BURST_ITERATOR_ITERATOR_VALUE_HPP

#include <iterator>

namespace burst
{
    template <typename Iterator>
    using iterator_value_t = typename std::iterator_traits<Iterator>::value_type;
}

#endif // BURST_ITERATOR_ITERATOR_VALUE_HPP
