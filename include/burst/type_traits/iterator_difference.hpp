#ifndef BURST__TYPE_TRAITS__ITERATOR_DIFFERENCE_HPP
#define BURST__TYPE_TRAITS__ITERATOR_DIFFERENCE_HPP

#include <iterator>

namespace burst
{
    template <typename Iterator>
    using iterator_difference_t = typename std::iterator_traits<Iterator>::difference_type;
}

#endif // BURST__TYPE_TRAITS__ITERATOR_DIFFERENCE_HPP
