#ifndef BURST_ITERATOR_ITERATOR_DIFFERENCE_HPP
#define BURST_ITERATOR_ITERATOR_DIFFERENCE_HPP

#include <iterator>

namespace burst
{
    template <typename Iterator>
    using iterator_difference_t = typename std::iterator_traits<Iterator>::difference_type;
}

#endif // BURST_ITERATOR_ITERATOR_DIFFERENCE_HPP
