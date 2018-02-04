#ifndef BURST_ITERATOR_TRAITS_ITERATOR_POINTER_HPP
#define BURST_ITERATOR_TRAITS_ITERATOR_POINTER_HPP

#include <iterator>

namespace burst
{
    template <typename Iterator>
    using iterator_pointer_t = typename std::iterator_traits<Iterator>::pointer;
}

#endif // BURST_ITERATOR_TRAITS_ITERATOR_POINTER_HPP
