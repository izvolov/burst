#ifndef BURST__ITERATOR__VARIANT_ITERATOR_HPP
#define BURST__ITERATOR__VARIANT_ITERATOR_HPP

#include <burst/type_traits/iterator_category.hpp>
#include <burst/type_traits/iterator_reference.hpp>
#include <burst/type_traits/iterator_value.hpp>
#include <burst/type_traits/minimum_category.hpp>
#include <burst/type_traits/one_of_equal.hpp>

#include <boost/iterator/iterator_facade.hpp>
#include <boost/variant/apply_visitor.hpp>
#include <boost/variant/get.hpp>
#include <boost/variant/variant.hpp>

#include <iterator>
#include <type_traits>
#include <utility>

namespace burst
{
    /*!
        \brief
            Итератор-вариант

        \details
            Является итератором, который может хранить в себе один из перечисленных в параметре
            Iterators.
            -   Тип итерируемых значений и результат разыменования идентичен исходным итераторам;
            -   Категория — минимум из категорий исходных итераторов.

        \tparam Iterators
            Типы возможных итераторов. Сами итераторы могут быть произвольными, но с оговоркой,
            что типы значений (iterator_value) и ссылок (iterator_reference) должны быть одинаковыми
            для всех.

        \see variant_range
     */
    template <typename... Iterators>
    class variant_iterator:
        public boost::iterator_facade
        <
            variant_iterator<Iterators...>,
            one_of_equal_t<iterator_value_t<Iterators>...>,
            typename boost::iterator_category_to_traversal
            <
                minimum_category_t<iterator_category_t<Iterators>...>
            >
            ::type,
            one_of_equal_t<iterator_reference_t<Iterators>...>
        >
    {
    private:
        using base_type =
            boost::iterator_facade
            <
                variant_iterator<Iterators...>,
                one_of_equal_t<iterator_value_t<Iterators>...>,
                typename boost::iterator_category_to_traversal
                <
                    minimum_category_t<iterator_category_t<Iterators>...>
                >
                ::type,
                one_of_equal_t<iterator_reference_t<Iterators>...>
            >;

    public:
        template <typename Iterator>
        explicit variant_iterator (Iterator i):
            m_iterator(std::move(i))
        {
        }

        variant_iterator () = default;

    private:
        friend class boost::iterator_core_access;

        void increment ()
        {
            boost::apply_visitor([] (auto & i) {++i;}, m_iterator);
        }

        void decrement ()
        {
            boost::apply_visitor([] (auto & i) {--i;}, m_iterator);
        }

        void advance (typename base_type::difference_type n)
        {
            boost::apply_visitor([n] (auto & i) {std::advance(i, n);}, m_iterator);
        }

        bool equal (const variant_iterator & that) const
        {
            const auto compare =
                [& that] (const auto & i)
                {
                    using actual_type = std::decay_t<decltype(i)>;
                    return i == boost::get<actual_type>(that.m_iterator);
                };
            return boost::apply_visitor(compare, m_iterator);
        }

        typename base_type::difference_type distance_to (const variant_iterator & that) const
        {
            const auto distance =
                [& that] (const auto & i)
                {
                    using actual_type = std::decay_t<decltype(i)>;
                    return boost::get<actual_type>(that.m_iterator) - i;
                };
            return boost::apply_visitor(distance, m_iterator);
        }

        typename base_type::reference dereference () const
        {
            return boost::apply_visitor([] (auto & i) -> decltype(auto) {return *i;}, m_iterator);
        }

    private:
        boost::variant<Iterators...> m_iterator;
    };
} // namespace burst

#endif // BURST__ITERATOR__VARIANT_ITERATOR_HPP
