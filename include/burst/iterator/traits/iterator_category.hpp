#ifndef BURST_ITERATOR_TRAITS_ITERATOR_CATEGORY_HPP
#define BURST_ITERATOR_TRAITS_ITERATOR_CATEGORY_HPP

#include <iterator>

namespace burst
{
    template <typename Iterator>
    using iterator_category_t = typename std::iterator_traits<Iterator>::iterator_category;
}

#endif // BURST_ITERATOR_TRAITS_ITERATOR_CATEGORY_HPP
