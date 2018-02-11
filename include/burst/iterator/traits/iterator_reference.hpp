#ifndef BURST_ITERATOR_TRAITS_ITERATOR_REFERENCE_HPP
#define BURST_ITERATOR_TRAITS_ITERATOR_REFERENCE_HPP

#include <iterator>

namespace burst
{
    template <typename Iterator>
    using iterator_reference_t = typename std::iterator_traits<Iterator>::reference;
}

#endif // BURST_ITERATOR_TRAITS_ITERATOR_REFERENCE_HPP
