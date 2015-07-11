#ifndef BURST_ITERATOR_BITAP_ITERATOR_HPP
#define BURST_ITERATOR_BITAP_ITERATOR_HPP

#include <iterator>
#include <type_traits>

#include <boost/iterator/iterator_facade.hpp>
#include <boost/range/concepts.hpp>
#include <boost/range/iterator_range.hpp>

#include <burst/algorithm/searching/bitap.hpp>
#include <burst/iterator/end_tag.hpp>

namespace burst
{
    //!     Итератор совпадений образца в тексте по алгоритму "bitap".
    /*!
            Обёртка, позволяющая ровно за один проход найти все вхождения заданного образца в
        некоторый "текст" (последовательность элементов).
            Поиск осуществляется при помощи двоичного алгоритма поиска подстроки.
     */
    template <typename Bitap, typename TextRange>
    class bitap_iterator:
        public boost::iterator_facade
        <
            bitap_iterator<Bitap, TextRange>,
            boost::iterator_range<typename TextRange::const_iterator>,
            boost::forward_traversal_tag,
            const boost::iterator_range<typename TextRange::const_iterator> &
        >
    {
    private:
        using searcher_type = Bitap;
        using bitmask_type = typename searcher_type::bitmask_type;

        BOOST_CONCEPT_ASSERT((boost::ForwardRangeConcept<TextRange>));
        using text_range_type = TextRange;
        using text_iterator = typename TextRange::const_iterator;
        using text_value_type = typename std::iterator_traits<text_iterator>::value_type;

        using base_type = boost::iterator_facade
        <
            bitap_iterator<searcher_type, text_range_type>,
            boost::iterator_range<typename text_range_type::const_iterator>,
            boost::forward_traversal_tag,
            const boost::iterator_range<typename text_range_type::const_iterator> &
        >;

    public:
        bitap_iterator (const searcher_type & bitap, const text_range_type & text):
            m_bitap(bitap),
            m_hint(0x00),
            m_match(m_bitap.find_first(text.begin(), text.end(), m_hint)),
            m_text_end(text.end())
        {
        }

        bitap_iterator (const searcher_type & bitap, const text_range_type & text, iterator::end_tag_t):
            m_bitap(bitap),
            m_hint(0x00),
            m_match(text.end(), text.end()),
            m_text_end(text.end())
        {
        }

    private:
        friend class boost::iterator_core_access;

        void increment ()
        {
            m_match = m_bitap.find_next(m_match.begin(), m_match.end(), m_text_end, m_hint);
        }

    private:
        typename base_type::reference dereference () const
        {
            return m_match;
        }

        bool equal (const bitap_iterator & that) const
        {
            return this->m_match.begin() == that.m_match.begin();
        }

    private:
        searcher_type m_bitap;
        bitmask_type m_hint;
        typename base_type::value_type m_match;
        text_iterator m_text_end;
    };

    //!     Функция создания итератора на начало совпадений.
    /*!
            Принимает на вход готовый объект, хранящий в закодированном виде образец, который нужно
        искать, а также диапазон, в котором и требуется найти вхождения образца.
            Возвращает итератор на первое вхождение образца в текст.
            Вхождение задаётся в виде диапазона из двух итераторов, который указывает на место в
        тексте, где встретился образец.
     */
    template <typename Value, typename Bitmask, typename Map, typename ForwardRange>
    bitap_iterator
    <
        algorithm::bitap<Value, Bitmask, Map>,
        ForwardRange
    >
    make_bitap_iterator (const algorithm::bitap<Value, Bitmask, Map> & bitap, const ForwardRange & text)
    {
        static_assert(std::is_same<Value, typename ForwardRange::value_type>::value, "");
        return bitap_iterator<algorithm::bitap<Value, Bitmask, Map>, ForwardRange>(bitap, text);
    }

    //!     Функция создания итератора на конец совпадений.
    /*!
            Возвращает итератор-метку конца совпадений.
            Если итератор совпадений равен этому итератору, то это значит, что совпадения
        закончились.
     */
    template <typename Value, typename Bitmask, typename Map, typename ForwardRange>
    bitap_iterator
    <
        algorithm::bitap<Value, Bitmask, Map>,
        ForwardRange
    >
    make_bitap_iterator
        (
            const algorithm::bitap<Value, Bitmask, Map> & bitap,
            const ForwardRange & text,
            iterator::end_tag_t
        )
    {
        static_assert(std::is_same<Value, typename ForwardRange::value_type>::value, "");
        return bitap_iterator<algorithm::bitap<Value, Bitmask, Map>, ForwardRange>(bitap, text, iterator::end_tag);
    }
}

#endif // BURST_ITERATOR_BITAP_ITERATOR_HPP
