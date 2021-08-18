#ifndef BURST__TYPE_TRAITS__ITERATOR_REFERENCE_HPP
#define BURST__TYPE_TRAITS__ITERATOR_REFERENCE_HPP

#include <iterator>

namespace burst
{
    template <typename Iterator>
    using iterator_reference_t = typename std::iterator_traits<Iterator>::reference;
}

#endif // BURST__TYPE_TRAITS__ITERATOR_REFERENCE_HPP
