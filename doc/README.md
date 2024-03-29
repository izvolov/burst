Справочник
----------

1.  [Алгоритмы](#algorithms)
    1.  [Целочисленные сортировки](#intsort)
        1.  [Поразрядная сортировка](#radix)
        2.  [Параллельная поразрядная сортировка](#radix-par)
        3.  [Сортировка подсчётом](#counting)
    2.  [Скачущий поиск](#gallop)
        1.  [Поиск нижней грани](#galloping-lb)
        2.  [Поиск верхней грани](#galloping-ub)
2.  [Структуры данных](#data-structures)
    1.  [Плоское k-местное дерево поиска](#kary)
    2.  [Динамический кортеж](#dynamic-tuple)
3.  [Ленивые вычисления](#lazy-ranges)
    1.  [Склейка](#join)
    2.  [Слияние](#merge)
    3.  [Пересечение](#intersect)
    4.  [Полупересечение](#semiintersect)
    5.  [Объединение](#union)
    6.  [Разность](#difference)
    7.  [Симметрическая разность](#symmetric-difference)
    8.  [Разбиение на куски](#buffered-chunks)
4.  [Рабочие инструменты](#utilities)
    1.  [Конструирование контейнеров с выводом типа](#inferring-constructors)

<a name="algorithms"/> Алгоритмы
--------------------------------

### <a name="intsort"/> Целочисленные сортировки

Классические алгоритмы эффективной сортировки целых чисел с линейным временем вычисления. Быстры даже на небольших массивах:

![Поразрядная сортировка быстра даже на небольших массивах](images/radix_sort_is_fast_on_small_arrays.png)

#### <a name="radix"/> Поразрядная сортировка

```cpp
std::vector<int> ints{4, 3, 2, 1};
std::vector<int> buffer(ints.size());

burst::radix_sort(ints.begin(), ints.end(), buffer.begin());

assert((ints == std::vector<int>{1, 2, 3, 4}));
```

#### <a name="radix-par"/> Параллельная поразрядная сортировка

```cpp
std::vector<int> ints{...};

std::vector<int> buffer(ints.size());
burst::radix_sort(burst::par(16), ints, buffer.begin());
```

Находится в заголовке
```cpp
#include <burst/algorithm/radix_sort.hpp>
```

#### <a name="counting"/> Сортировка подсчётом

```cpp
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
```cpp
#include <burst/algorithm/counting_sort.hpp>
```

### <a name="gallop"/> Скачущий поиск

Эффективен и обгоняет двоичный поиск в том случае, если искомый элемент находится близко к началу
диапазона.

#### <a name="galloping-lb"/> Поиск нижней грани

```cpp
std::vector<int> range{1, 2, 2, 3, 3, 3};
//                              ^

auto search_result = burst::galloping_lower_bound(range.begin(), range.end(), 3);

assert(search_result == range.begin() + 3);
assert(*search_result == 3);
```

В заголовке
```cpp
#include <burst/algorithm/galloping_lower_bound.hpp>
```

#### <a name="galloping-ub"/> Поиск верхней грани

```cpp
std::vector<int> range{30, 30, 30, 20, 20, 10};
//                                         ^

auto search_result =
    burst::galloping_upper_bound(range.begin(), range.end(), 20, std::greater<>{});

assert(search_result == range.begin() + 5);
assert(*search_result == 10);
```

В заголовке
```cpp
#include <burst/algorithm/galloping_upper_bound.hpp>
```

<a name="data-structures"/> Структуры данных
--------------------------------------------

### <a name="kary"/> Плоское k-местное дерево поиска

На достаточно большом количестве данных опережает по скорости поиска и `std::set`, и `std::unordered_set`, и `boost::container::flat_set`.

Скорость достигается за счёт удобной для процессорного кэша упаковки данных.

```cpp
burst::k_ary_search_set<int> set{3, 4, 6, 1, 7, 8, 2};
assert(*set.find(3) == 3);
assert(set.find(0) == set.end());
```

В заголовке
```cpp
#include <burst/container/k_ary_search_set.hpp>
```

### <a name="dynamic-tuple"/> Динамический кортеж

Неоднородный контейнер с плотной упаковкой.
Ключевые особенности:
1. Складываемые в контейнер типы не обязаны быть членами одной иерархии классов — это могут быть совершенно разные типы. Например, можно поместить одновременно `bool`, `double`, `std::string`, `std::vector<int>` и т.п.
2. Объекты плотно расположены в памяти. То есть они лежат в едином буфере подряд, друг за другом. Это снимает лишний уровень косвенности при обращении к ним по сравнению с тем, если бы это был, например, массив указателей на базовый класс, как это обычно делается для хранения в массиве классов из одной иерархии.

Объект можно получить зная его тип и индекс в контейнере.
```cpp
burst::dynamic_tuple t(std::vector<int>{1, 2, 3});

t.push_back(true);
t.push_back(std::string("123"));

assert(t.size() == 3);

assert(t.get<std::vector<int>>(0) == (std::vector<int>{1, 2, 3}));
assert(t.get<bool>(1) == true);
assert(t.get<std::string>(2) == std::string("123"));
```

В заголовке
```cpp
#include <burst/container/dynamic_tuple.hpp>
```

<a name="lazy-ranges"/> Ленивые вычисления
------------------------------------------

Операции с диапазонами без создания дополнительного буфера для хранения их содержимого.

### <a name="join"/> Склейка

Представляет набор диапазонов таким образом, как будто это единый диапазон.

```cpp
std::string hello("hello");
std::string world("world");

auto helloworld = burst::join(std::tie(hello, world));

assert(helloworld == std::string("helloworld"));
```

В заголовке
```cpp
#include <burst/range/join.hpp>
```

### <a name="merge"/> Слияние

Производит ленивое слияние набора упорядоченных множеств.
Создаёт упорядоченный диапазон, пробегающийся по всем элементам всех входных множеств.

Созданный диапазон однопроходный (SinglePass в терминологии Буста).

```cpp
std::vector<int> even{   2,    4,    6};
std::vector<int>  odd{1,    3,    5   };
//                    ^  ^  ^  ^  ^  ^

auto merged_range = burst::merge(std::tie(even, odd));

auto expected_collection = {1, 2, 3, 4, 5, 6};
assert(merged_range == expected_collection);
```

В заголовке
```cpp
#include <burst/range/merge.hpp>
```

### <a name="intersect"/> Пересечение

Производит ленивое пересечение набора упорядоченных множеств.
Создаёт упорядоченный диапазон, пробегающийся по тем элементам, которые есть одновременно во всех входных множествах.

Созданный диапазон однопроходный (SinglePass в терминологии Буста).

```cpp
std::vector<int> natural{1, 2, 3, 4, 5, 6, 7};
std::vector<int>   prime{   2, 3,    5,    7};
std::vector<int>     odd{1,    3,    5,    7};
//                             ^     ^     ^

auto intersected_range = burst::intersect(std::tie(natural, prime, odd));

auto expected_collection = {3, 5, 7};
assert(intersected_range == expected_collection);
```

В заголовке
```cpp
#include <burst/range/intersect.hpp>
```

### <a name="semiintersect"/> Полупересечение

`m`-полупересечение нескольких множеств содержит те элементы, которые содержатся не менее, чем в `m` из этих множеств.

Производит ленивое полупересечение набора упорядоченных множеств.
Создаёт упорядоченный диапазон, пробегающийся по всем элементам, которые есть не менее, чем в `m` входных множествах.

Созданный диапазон однопроходный (SinglePass в терминологии Буста).

```cpp
auto  first = {0, 0, 1,       2};
auto second = {0,    1, 1      };
auto  third = {      1, 1, 1, 2};
//             ^     ^  ^     ^

auto semiintersection = burst::semiintersect(std::tie(first, second, third), 2);

auto expected_collection = {0, 1, 1, 2};
assert(semiintersection == expected_collection);
```

В заголовке
```cpp
#include <burst/range/semiintersect.hpp>
```

### <a name="union"/> Объединение

Производит ленивое объединение набора упорядоченных множеств.
Создаёт упорядоченный диапазон, пробегающийся по всем элементам входных множеств без учёта повторяющихся элементов.

Созданный диапазон однопроходный (SinglePass в терминологии Буста).

```cpp
std::vector<int>   one{1, 2      };
std::vector<int>   two{   2, 3   };
std::vector<int> three{      3, 4};
//                     ^  ^  ^  ^

auto range_union = burst::unite(std::tie(one, two, three));

auto expected_collection = {1, 2, 3, 4};
assert(range_union == expected_collection);
```

В заголовке
```cpp
#include <burst/range/unite.hpp>
```

### <a name="difference"/> Разность

Производит ленивую разность двух упорядоченных множеств.
Создаёт упорядоченный диапазон, пробегающийся по всем элементам, которые есть в первом множестве, но которых нет во втором.

```cpp
const auto natural = burst::make_vector({1, 2, 3, 4, 5, 6});
const auto     odd = burst::make_vector({1,    3,    5   });
//                                          ^     ^     ^

const auto difference = burst::difference(natural, odd);

auto even = {2, 4, 6};
assert(difference == even);
```

В заголовке
```cpp
#include <burst/range/difference.hpp>
```

### <a name="symmetric-difference"/> Симметрическая разность

Производит ленивую симметрическую разность набора упорядоченных множеств.
Создаёт упорядоченный диапазон, пробегающийся по всем элементам, которые есть в нечётном числе входных множеств.

Созданный диапазон однопроходный (SinglePass в терминологии Буста).

```cpp
const auto  first = burst::make_vector({1, 2, 3, 4      });
const auto second = burst::make_vector({1, 2, 3,    5, 5});
const auto  third = burst::make_vector({1,    3, 4, 5   });
const auto fourth = burst::make_vector({1, 2,    4      });
//                                         ^  ^  ^     ^

const auto difference =
    burst::symmetric_difference(std::tie(first, second, third, fourth));

auto result = {2, 3, 4, 5};
assert(difference == result);
```

В заголовке
```cpp
#include <burst/range/symmetric_difference.hpp>
```

### <a name="buffered-chunks"/> Разбиение на куски

Разбивает входную последовательность элементов на куски равного размера (последний может быть меньше), причём текущий кусок копируется во внутренний буфер и хранится в нём. Исходная последовательность воздействию не подвергается.

Созданный диапазон однопроходный (SinglePass в терминологии Буста).

```cpp
const auto sequence =
    burst::make_vector
    ({
        1, 2, 3, 4, // Первый кусок
        5, 6, 7, 8, // Второй кусок
        9           // Третий кусок
    });

auto chunks = burst::buffered_chunks(sequence, 4);

const auto first_chunk = {1, 2, 3, 4};
assert(chunks.front() == first_chunk);

chunks.advance_begin(1);
const auto second_chunk = {5, 6, 7, 8};
assert(chunks.front() == second_chunk);

chunks.advance_begin(1);
const auto third_chunk = {9};
assert(chunks.front() == third_chunk);
```

В заголовке
```cpp
#include <burst/range/buffered_chunks.hpp>
```

<a name="utilities"/> Рабочие инструменты
-----------------------------------------

### <a name="inferring-constructors"/> Конструирование контейнеров с выводом типа

Часто бывает так, что нужно сохранить данные в контейнер, но при этом тип этих данных либо заранее неизвестен, либо его сложно описать, либо он просто не важен.

В этом случае полезно иметь функцию, выполняющую конструирование этого контейнера так, что тип контейнера выведется автоматически из входных аргументов функции, и пользователю не придётся задавать этот тип вручную.

#### Конструирование из списка инициализации

```cpp
auto v = burst::make_vector({1, 2, 3, 4});
```

#### Конструирование при помощи двух итераторов

```cpp
auto v = burst::make_vector(collection.begin() + 3, collection.end());
```

#### Конструирование из диапазона

```cpp
auto v = burst::make_vector(collection);
```

#### Заполняющее конструирование

```cpp
auto v = burst::make_vector(5, x);
```

Доступны практически все варианты конструкторов, а также варианты с конструированием из диапазона для стандартных последовательных контейнеров (`std::vector`, `std::deque`, `std::list`, `std::forward_list`).

В заголовках
```cpp
#include <burst/container/make_deque.hpp>
#include <burst/container/make_forward_list.hpp>
#include <burst/container/make_list.hpp>
#include <burst/container/make_vector.hpp>
```

Кроме того, поддерживается любой совместимый сторониий контейнер при помощи функции `make_sequence_container`:
```cpp
auto c = burst::make_sequence_container<boost::container::vector>(...);
```

В заголовке
```cpp
#include <burst/container/make_sequence_container.hpp>
```
