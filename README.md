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

1. [Структура для быстрого поиска в больших массивах данных](#kary)
2. [Работа с диапазонами "на лету"](#ranges)
   1. [Склейка](#join)
   2. [Слияние](#merge)
   3. [Пересечение](#intersect)
3. [Целочисленная сортировка](#intsort)
   1. [Сортировка подсчётом](#counting)
   2. [Копирование диапазона, отсортированного подсчётом](#counting-copy)
   3. [Поразрядная сортировка](#radix)
   4. [Копирование диапазона, отсортированного поразрядной сортировкой](#radix-copy)
4. [Скачущий поиск](#gallop)
   1. [Поиск нижней грани](#galloping-lb)
   2. [Поиск верхней грани](#galloping-ub)

#### <a name="kary"/> Структура для быстрого поиска в больших массивах данных

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

#### <a name="ranges"/> Работа с диапазонами "на лету"

Операции с диапазонами без создания дополнительного буфера для хранения их содержимого.

* <a name="join"/> Склейка
  
  ```c++
  std::string hello("hello");
  std::string world("world");

  auto helloworld = burst::join
  ({
      boost::make_iterator_range(hello),
      boost::make_iterator_range(world)
  });

  assert(helloworld == std::string("helloworld"));
  ```

  В заголовке
  ```c++
  #include <burst/range/join.hpp>
  ```
  
* <a name="merge"/> Слияние
  
  ```c++
  std::vector<int> even{2, 4, 6};
  std::vector<int>  odd{1, 3, 5};

  auto merged_range = burst::merge
  ({
      boost::make_iterator_range(even),
      boost::make_iterator_range(odd)
  });

  assert((merged_range == std::vector<int>{1, 2, 3, 4, 5, 6}));
  ```

  В заголовке
  ```c++
  #include <burst/range/merge.hpp>
  ```
  
* <a name="intersect"/> Пересечение
  
  ```c++
  std::vector<int> natural{1, 2, 3, 4, 5, 6, 7};
  std::vector<int> prime{2, 3, 5, 7};
  std::vector<int> odd{1, 3, 5, 7};

  auto intersected_range = burst::intersect
  ({
      boost::make_iterator_range(natural),
      boost::make_iterator_range(prime),
      boost::make_iterator_range(odd)
  });

  assert((intersected_range == std::vector<int>{3, 5, 7}));
  ```

  В заголовке
  ```c++
  #include <burst/range/intersect.hpp>
  ```
  
#### <a name="intsort"/> Целочисленная сортировка

* <a name="counting"/> Сортировка подсчётом

  ```c++
  std::vector<std::int8_t> numbers{0, -1, 1, -2, 2};

  burst::counting_sort(numbers.begin(), numbers.end());

  assert((numbers == std::vector<std::int8_t>{-2, -1, 0, 1, 2}));
  ```

  Находится в заголовке
  ```c++
  #include <burst/algorithm/sorting/counting_sort.hpp>
  ```
  
* <a name="counting-copy"/> Копирование диапазона, отсортированного подсчётом

  ```c++
  std::vector<std::size_t> chaos{0x0104, 0x0203, 0x0302, 0x0401};

  std::vector<std::size_t> sorted_by_low_byte(chaos.size());
  burst::counting_sort_copy(chaos.begin(), chaos.end(), sorted_by_low_byte.begin(),
      [] (const std::size_t & integer) -> std::uint8_t
      {
          return integer & 0xff;
      });
      
  assert((sorted_by_low_byte == std::vector<std::size_t>{0x0401, 0x0302, 0x0203, 0x0104}));
  ```

  Находится в заголовке
  ```c++
  #include <burst/algorithm/sorting/counting_sort_copy.hpp>
  ```

* <a name="radix"/> Поразрядная сортировка
  ```c++
  std::vector<std::string> strings{"aaaa", "bbb", "cc", "d"};

  burst::radix_sort(strings.begin(), strings.end(),
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

* <a name="radix-copy"/> Копирование диапазона, отсортированного поразрядной сортировкой
  ```c++
  std::vector<std::uint32_t> chaos{100500, 42, 99999, 1000, 0};

  std::forward_list<std::uint32_t> order(chaos.size());
  burst::radix_sort_copy(chaos.begin(), chaos.end(), order.begin());

  assert((order == std::forward_list<std::uint32_t>{0, 42, 1000, 99999, 100500}));
  ```

  Находится в заголовке
  ```c++
  #include <burst/algorithm/sorting/radix_sort_copy.hpp>
  ```

#### <a name="gallop"/> Скачущий поиск

Эффективен и обгоняет двоичный поиск в том случае, если искомый элемент находится близко к началу
диапазона.

* <a name="galloping-lb"/> Поиск нижней грани
  ```c++
  std::vector<int> range{1, 2, 2, 3, 3, 3};

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

  auto search_result = burst::galloping_upper_bound(range.begin(), range.end(), 20, std::greater<int>());

  assert(search_result == range.begin() + 5);
  assert(*search_result == 10);
  ```

  В заголовке
  ```c++
  #include <burst/algorithm/galloping_upper_bound.hpp>
  ```

Требования
----------

Проект работает с компиляторами

- clang 3.5
- GCC 4.9.2

и с библиотекой Boost начиная с версии

- 1.57.0

Про более древние компиляторы и версии Буста мне ничего неизвестно.

[![Статус сборки](https://travis-ci.org/izvolov/burst.svg?branch=master)](https://travis-ci.org/izvolov/burst)
