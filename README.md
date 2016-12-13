Burst
=====

То, чего нет в Бусте.

О проекте
---------

Стандартная библиотека неплоха, но в ней кое-чего не хватает. Поэтому умные люди написали набор библиотек, которые дополняли бы стандартную тем, чего в ней нет.
Однако, и в этих дополнительных библиотеках тоже кое-чего не хватает.

В данном проекте я попытался собрать некоторые алгоритмы и инструменты, совместимые с СБШ и Бустом, которые нужны в повседневной работе, но которых пока нет ни в стандартной библиотеке, ни в библиотеке Boost.

Что интересного?
----------------

1. [Алгоритмы](#algorithms)
    1. [Целочисленные сортировки](#intsort)
        1. [Поразрядная сортировка](#radix)
        2. [Сортировка подсчётом](#counting)
    2. [Скачущий поиск](#gallop)
        1. [Поиск нижней грани](#galloping-lb)
        2. [Поиск верхней грани](#galloping-ub)
1. [Структуры данных](#data-structures)
    1. [Структура для быстрого поиска в больших массивах данных](#kary)
    2. [Динамический кортеж](#dynamic-tuple)
2. [Ленивые вычисления](#lazy-ranges)
    1. [Склейка](#join)
    2. [Слияние](#merge)
    3. [Пересечение](#intersect)
    4. [Полупересечение](#semiintersect)
    5. [Объединение](#union)
    6. [Разность](#difference)

### <a name="algorithms"/> Алгоритмы

#### <a name="intsort"/> Целочисленные сортировки

Классические алгоритмы эффективной сортировки целых чисел с линейным временем вычисления.

* <a name="radix"/> Поразрядная сортировка

    ```c++
    std::vector<std::string> strings{"aaaa", "bbb", "cc", "d"};

    std::vector<std::string> buffer(strings.size());
    burst::radix_sort(strings.begin(), strings.end(), buffer.begin(),
        [] (const std::string & string)
        {
            return string.size();
        }
    );
    assert((strings == std::vector<std::string>{"d", "cc", "bbb", "aaaa"}));
    ```

    Находится в заголовке
    ```c++
    #include <burst/algorithm/sorting/radix_sort.hpp>
    ```

* <a name="counting"/> Сортировка подсчётом

    ```c++
    std::vector<std::size_t> chaos{0x0104, 0x0203, 0x0302, 0x0401};

    std::vector<std::size_t> sorted_by_low_byte(chaos.size());
    burst::counting_sort(chaos.begin(), chaos.end(), sorted_by_low_byte.begin(),
        [] (const std::size_t & integer) -> std::uint8_t
        {
            return integer & 0xff;
        });

    assert((sorted_by_low_byte == std::vector<std::size_t>{0x0401, 0x0302, 0x0203, 0x0104}));
    ```

    Находится в заголовке
    ```c++
    #include <burst/algorithm/sorting/counting_sort.hpp>
    ```

#### <a name="gallop"/> Скачущий поиск

Эффективен и обгоняет двоичный поиск в том случае, если искомый элемент находится близко к началу
диапазона.

* <a name="galloping-lb"/> Поиск нижней грани

    ```c++
    std::vector<int> range{1, 2, 2, 3, 3, 3};
    //                              ^

    auto search_result = burst::galloping_lower_bound(range.begin(), range.end(), 3);

    assert(search_result == range.begin() + 3);
    assert(*search_result == 3);
    ```

    В заголовке
    ```c++
    #include <burst/algorithm/galloping_lower_bound.hpp>
    ```

* <a name="galloping-ub"/> Поиск верхней грани

    ```c++
    std::vector<int> range{30, 30, 30, 20, 20, 10};
    //                                         ^

    auto search_result =
        burst::galloping_upper_bound(range.begin(), range.end(), 20, std::greater<>{});

    assert(search_result == range.begin() + 5);
    assert(*search_result == 10);
    ```

    В заголовке
    ```c++
    #include <burst/algorithm/galloping_upper_bound.hpp>
    ```

### <a name="data-structures"/> Структуры данных

* <a name="kary"/> Структура для быстрого поиска в больших массивах данных

    На достаточно большом количестве данных опережает по скорости поиска и ```std::set```, и ```std::unordered_set```, и ```boost::container::flat_set```.

    ```c++
    burst::k_ary_search_set<int> set{3, 4, 6, 1, 7, 8, 2};
    assert(*set.find(3) == 3);
    assert(set.find(0) == set.end());
    ```

    В заголовке
    ```c++
    #include <burst/container/k_ary_search_set.hpp>
    ```

* <a name="dynamic-tuple"/> Динамический кортеж

    Неоднородный контейнер с плотной упаковкой.
    Ключевые особенности:
    1. Складываемые в контейнер типы не обязаны быть членами одной иерархии классов — это могут быть совершенно разные типы. Например, можно поместить одновременно ```bool```, ```double```, ```std::string```, ```std::vector<int>``` и т.п.
    2. Объекты плотно расположены в памяти. То есть они лежат в едином буфере подряд, друг за другом. Это снимает лишний уровень косвенности при обращении к ним по сравнению с тем, если бы это был, например, массив указателей на базовый класс, как это обычно делается для хранения в массиве классов из одной иерархии.

    Объект можно получить зная его тип и индекс в контейнере.
    ```c++
    burst::dynamic_tuple t(std::vector<int>{1, 2, 3});

    t.push_back(true);
    t.push_back(std::string("123"));

    assert(t.size() == 3);

    assert(t.get<std::vector<int>>(0) == (std::vector<int>{1, 2, 3}));
    assert(t.get<bool>(1) == true);
    assert(t.get<std::string>(2) == std::string("123"));
    ```

    В заголовке
    ```c++
    #include <burst/container/dynamic_tuple.hpp>
    ```

### <a name="lazy-ranges"/> Ленивые вычисления

Операции с диапазонами без создания дополнительного буфера для хранения их содержимого.

* <a name="join"/> Склейка
  
    ```c++
    std::string hello("hello");
    std::string world("world");
    auto ranges = burst::make_range_vector(hello, world);

    auto helloworld = burst::join(ranges);

    assert(helloworld == std::string("helloworld"));
    ```

    В заголовке
    ```c++
    #include <burst/range/join.hpp>
    ```
  
* <a name="merge"/> Слияние
  
    ```c++
    std::vector<int> even{   2,    4,    6};
    std::vector<int>  odd{1,    3,    5   };
    //                    ^  ^  ^  ^  ^  ^
    auto ranges = burst::make_range_vector(even, odd);

    auto merged_range = burst::merge(ranges);

    assert((merged_range == std::vector<int>{1, 2, 3, 4, 5, 6}));
    ```

    В заголовке
    ```c++
    #include <burst/range/merge.hpp>
    ```
  
* <a name="intersect"/> Пересечение
  
    ```c++
    std::vector<int> natural{1, 2, 3, 4, 5, 6, 7};
    std::vector<int>   prime{   2, 3,    5,    7};
    std::vector<int>     odd{1,    3,    5,    7};
    //                             ^     ^     ^
    auto ranges = burst::make_range_vector(natural, prime, odd);

    auto intersected_range = burst::intersect(ranges);

    assert((intersected_range == std::vector<int>{3, 5, 7}));
    ```

    В заголовке
    ```c++
    #include <burst/range/intersect.hpp>
    ```

* <a name="semiintersect"/> Полупересечение

    ```c++
    auto  first = {0, 0, 1,       2};
    auto second = {0,    1, 1      };
    auto  third = {      1, 1, 1, 2};
    //             ^     ^  ^     ^
    auto ranges = burst::make_range_vector(first, second, third);

    auto semiintersection = burst::semiintersect(ranges, 2);

    auto expected_collection = {0, 1, 1, 2};
    assert(semiintersection == expected_collection);
    ```

    В заголовке
    ```c++
    #include <burst/range/semiintersect.hpp>
    ```

* <a name="union"/> Объединение
  
    ```c++
    std::vector<int>   one{1, 2      };
    std::vector<int>   two{   2, 3   };
    std::vector<int> three{      3, 4};
    //                     ^  ^  ^  ^
    auto ranges = burst::make_range_vector(one, two, three);

    auto range_union = burst::unite(ranges);

    assert((range_union == std::vector<int>{1, 2, 3, 4}));
    ```

    В заголовке
    ```c++
    #include <burst/range/unite.hpp>
    ```

* <a name="difference"/> Разность

    ```c++
    const auto natural = burst::make_vector({1, 2, 3, 4, 5, 6});
    const auto     odd = burst::make_vector({1,    3,    5   });
    //                                          ^     ^     ^

    const auto difference = burst::difference(natural, odd);

    const auto even = burst::make_vector({2, 4, 6});
    assert(difference == even);
    ```

    В заголовке
    ```c++
    #include <burst/range/difference.hpp>
    ```

Требования
----------

Проект работает с компиляторами

- clang 3.7
- GCC 6.2

и с библиотекой Boost начиная с версии

- 1.59.0

Про более древние компиляторы и версии Буста мне ничего неизвестно.

[![Статус сборки](https://travis-ci.org/izvolov/burst.svg?branch=master)](https://travis-ci.org/izvolov/burst)
