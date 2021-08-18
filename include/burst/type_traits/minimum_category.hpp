#ifndef BURST__TYPE_TRAITS__MINIMUM_CATEGORY_HPP
#define BURST__TYPE_TRAITS__MINIMUM_CATEGORY_HPP

#include <iterator>
#include <type_traits>

namespace burst
{
    /*!
        \brief
            Вычисляет меньшую категорию итератора из заданных

        \details
            Категории стандартной библиотеки по возрастанию:
            1. input_iterator_tag
            2. forward_iterator_tag
            3. bidirectional_iterator_tag
            4. random_access_iterator_tag
            5. contiguous_iterator_tag [C++20]
    */
    template <typename... Categories>
    struct minimum_category;

    template <typename... Categories>
    using minimum_category_t = typename minimum_category<Categories...>::type;

    template <typename Category, typename... Categories>
    struct minimum_category<Category, Categories...>
    {
        static_assert(std::is_convertible<Category, std::input_iterator_tag>::value, "");
        using type = std::common_type_t<Category, minimum_category_t<Categories...>>;
    };

    template <typename Category>
    struct minimum_category<Category>
    {
        static_assert(std::is_convertible<Category, std::input_iterator_tag>::value, "");
        using type = Category;
    };
} // namespace burst

#endif // BURST__TYPE_TRAITS__MINIMUM_CATEGORY_HPP
