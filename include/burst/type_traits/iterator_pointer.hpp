#ifndef BURST__TYPE_TRAITS__ITERATOR_POINTER_HPP
#define BURST__TYPE_TRAITS__ITERATOR_POINTER_HPP

#include <iterator>

namespace burst
{
    template <typename Iterator>
    using iterator_pointer_t = typename std::iterator_traits<Iterator>::pointer;
}

#endif // BURST__TYPE_TRAITS__ITERATOR_POINTER_HPP
