#include <burst/iterator/variant_iterator.hpp>
#include <burst/type_traits/iterator_category.hpp>

#include <doctest/doctest.h>

#include <boost/range/concepts.hpp>

#include <deque>
#include <forward_list>
#include <iterator>
#include <list>
#include <type_traits>
#include <vector>

TEST_SUITE("variant_iterator")
{
    TEST_CASE("Наследует минимальную из категорий переданных итераторов")
    {
        using forward_list_iterator = std::forward_list<int>::iterator;
        using list_iterator = std::list<int>::iterator;
        using deque_iterator = std::deque<int>::iterator;
        using vector_iterator = std::vector<int>::iterator;

        using variant_iterator =
            burst::variant_iterator
            <
                forward_list_iterator,
                list_iterator,
                deque_iterator,
                vector_iterator
            >;
        using category = burst::iterator_category_t<variant_iterator>;

        CHECK(std::is_same<category, burst::iterator_category_t<forward_list_iterator>>::value);
    }

    TEST_CASE("Наследует модель самого слабого итератора")
    {
        using forward_list_iterator = std::forward_list<int>::iterator;
        using list_iterator = std::list<int>::iterator;
        using deque_iterator = std::deque<int>::iterator;
        using vector_iterator = std::vector<int>::iterator;

        // Однонаправленный итератор
        {
        using variant_iterator =
            burst::variant_iterator
            <
                forward_list_iterator,
                list_iterator,
                deque_iterator,
                vector_iterator
            >;
            BOOST_CONCEPT_ASSERT((boost::ForwardIterator<variant_iterator>));
        }
        // Двунаправленный итератор
        {
            using variant_iterator =
                burst::variant_iterator<list_iterator, deque_iterator, vector_iterator>;
            BOOST_CONCEPT_ASSERT((boost::BidirectionalIterator<variant_iterator>));
        }
        // Итератор произвольного доступа
        {
            using variant_iterator = burst::variant_iterator<deque_iterator, vector_iterator>;
            BOOST_CONCEPT_ASSERT((boost::RandomAccessIterator<variant_iterator>));
        }
    }

    TEST_CASE("Параллелен обёрнутому итератору")
    {
        using list_iterator = std::list<int>::const_iterator;
        using vector_iterator = std::vector<int>::const_iterator;

        const auto d = std::list<int>{1, 2, 3};
        auto raw = d.begin();
        auto wrapped = burst::variant_iterator<list_iterator, vector_iterator>(raw);

        CHECK(*wrapped == *raw);

        ++wrapped;
        ++raw;
        CHECK(*wrapped == *raw);
    }

    TEST_CASE("Обладает конструктором по умолчанию")
    {
        using forward_list_iterator = std::forward_list<int>::const_iterator;
        using list_iterator = std::list<int>::const_iterator;
        using deque_iterator = std::deque<int>::const_iterator;
        using vector_iterator = std::vector<int>::const_iterator;

        using variant_iterator =
            burst::variant_iterator
            <
                forward_list_iterator,
                list_iterator,
                deque_iterator,
                vector_iterator,
                const int *
            >;

        auto i = variant_iterator{};
        static_cast<void>(i);
    }

    TEST_CASE("Допускает запись в неконстантный итератор")
    {
        using list_iterator = std::list<long>::iterator;
        using vector_iterator = std::vector<long>::iterator;

        auto l = std::list<long>{1, 2, 3};
        auto raw = l.begin();
        auto wrapped = burst::variant_iterator<list_iterator, vector_iterator>(raw);

        *wrapped = 111;
        CHECK(l.front() == 111);
    }

    TEST_CASE("Копируется как обычный итератор")
    {
        using list_iterator = std::list<int>::iterator;
        using deque_iterator = std::deque<int>::iterator;

        auto d = std::deque<int>{1, 2, 3};
        auto original = burst::variant_iterator<list_iterator, deque_iterator>(d.begin());
        auto copy = original;

        *original = 17;

        CHECK(*copy == 17);
    }

    TEST_CASE("Итератор произвольного доступа умеет продвигаться на N элементов вперёд и назад")
    {
        using vector_iterator = std::vector<char>::iterator;
        using deque_iterator = std::deque<char>::iterator;
        auto v = std::vector<char>{'a', 'b', 'c', 'd'};

        auto iterator = burst::variant_iterator<vector_iterator, deque_iterator>(v.begin());

        iterator += 3;
        CHECK(*iterator == 'd');

        iterator -= 2;
        CHECK(*iterator == 'b');
    }

    TEST_CASE("Итератор произвольного доступа допускает вычисление расстояния при помощи разности")
    {
        using vector_iterator = std::vector<char>::iterator;
        using deque_iterator = std::deque<char>::iterator;
        auto v = std::vector<char>{'a', 'b', 'c', 'd'};

        auto one = burst::variant_iterator<vector_iterator, deque_iterator>(v.begin());

        const auto another = std::next(one, 3);
        CHECK(another - one == 3);
    }

    TEST_CASE("Двунаправленный владеющий итератор умеет продвигаться вперёд и назад")
    {
        using list_iterator = std::list<char>::iterator;
        auto l = std::list<char>{'a', 'b', 'c', 'd'};

        auto iterator = burst::variant_iterator<list_iterator, char *>(l.begin());

        ++iterator;
        CHECK(*iterator == 'b');

        iterator++;
        CHECK(*iterator == 'c');

        --iterator;
        CHECK(*iterator == 'b');

        iterator--;
        CHECK(*iterator == 'a');
    }
}
