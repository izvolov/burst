#ifndef BURST__TYPE_TRAITS__ITERATOR_CATEGORY_HPP
#define BURST__TYPE_TRAITS__ITERATOR_CATEGORY_HPP

#include <iterator>

namespace burst
{
    template <typename Iterator>
    using iterator_category_t = typename std::iterator_traits<Iterator>::iterator_category;
}

#endif // BURST__TYPE_TRAITS__ITERATOR_CATEGORY_HPP
