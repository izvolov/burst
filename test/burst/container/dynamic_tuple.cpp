#include <utility/io/vector.hpp>

#include <burst/container/dynamic_tuple.hpp>
#include <burst/string/u8s.hpp>

#include <doctest/doctest.h>

#include <string>
#include <vector>

using burst::literals::operator ""_u8s;

namespace // anonymous
{
    struct X
    {
        int a = 0;
        std::vector<int> b = std::vector<int>{0};
    };

    struct dummy
    {
        dummy ():
            alive(true)
        {
            ++instances_count;
        }

        dummy (const dummy &):
            alive(true)
        {
            ++instances_count;
        }

        dummy (dummy &&):
            alive(true)
        {
            ++instances_count;
        }

        ~dummy () noexcept(false)
        {
            if (alive)
            {
                alive = false;
            }
            else
            {
                throw std::runtime_error(u8"Двойное уничтожение!"_u8s);
            }
            --instances_count;
        }

        bool alive;
        static int instances_count;
    };

    int dummy::instances_count = 0;

    struct kamikaze
    {
        kamikaze ()
        {
            ++instances_count;
        }

        kamikaze (const kamikaze &) = delete;

        kamikaze (kamikaze &&)
        {
            if (instances_count > 4)
            {
                throw std::runtime_error(u8"Привет!"_u8s);
            }
            ++instances_count;
        }

        ~kamikaze ()
        {
            --instances_count;
        }

        static int instances_count;
    };

    int kamikaze::instances_count = 0;

    struct move_constructor_counter
    {
        explicit move_constructor_counter (std::size_t & move_count):
            move_count(move_count)
        {
        }

        move_constructor_counter (move_constructor_counter && that):
            move_count(that.move_count)
        {
            ++move_count;
        }

        std::size_t & move_count;
    };

    struct destructor_counter
    {
        explicit destructor_counter (std::size_t & destruct_count):
            destruct_count(destruct_count)
        {
        }

        ~destructor_counter ()
        {
            ++destruct_count;
        }

        std::size_t & destruct_count;
    };
} // namespace anonymous

TEST_SUITE("dynamic_tuple")
{
    TEST_CASE("ДК может быть создан из произвольного набора объектов")
    {
        burst::dynamic_tuple t(std::string("123"), 42, true, X{17, std::vector<int>{1, 2, 3}});
        CHECK(t.size() == 4);
        CHECK(t.get<std::string>(0) == std::string("123"));
        CHECK(t.get<int>(1) == 42);
        CHECK(t.get<bool>(2) == true);
        CHECK(t.get<X>(3).a == 17);
        CHECK(t.get<X>(3).b == std::vector<int>{1, 2, 3});
    }

    TEST_CASE("ДК, созданный из пустого множества элементов, пуст")
    {
        const auto t = burst::dynamic_tuple{};
        CHECK(t.empty());
    }

    TEST_CASE("Размер ДК, созданного из пустого множества элементов, равен нулю")
    {
        const auto t = burst::dynamic_tuple{};
        CHECK(t.size() == 0);
    }

    TEST_CASE("Объём ДК, созданного из пустого множества элементов, равен нулю")
    {
        const auto t = burst::dynamic_tuple{};
        CHECK(t.volume() == 0);
    }

    TEST_CASE("Вместимость ДК, созданного из пустого множества элементов, равна нулю")
    {
        const auto t = burst::dynamic_tuple{};
        CHECK(t.capacity() == 0);
    }

    TEST_CASE("ДК, созданный из непустого множества элементов, непуст")
    {
        const auto t = burst::dynamic_tuple(42, 3.14);
        CHECK(not t.empty());
    }

    TEST_CASE("Размер ДК, созданного из непустого множества элементов, равен количеству этих "
        "элементов")
    {
        const auto t = burst::dynamic_tuple(std::string("123"), 42, 2.71, false);
        CHECK(t.size() == 4);
    }

    TEST_CASE("Объём ДК, созданного из некоторого множества элементов, равен объёму ДК, созданного "
        "последовательной вставкой тех же элементов в конец пустого ДК")
    {
        const auto constructed = burst::dynamic_tuple(42, 3.14, std::string("123"), true);

        auto pushed_back = burst::dynamic_tuple{};
        pushed_back.push_back(42);
        pushed_back.push_back(3.14);
        pushed_back.push_back(std::string("123"));
        pushed_back.push_back(true);

        CHECK(constructed.volume() == pushed_back.volume());
    }

    TEST_CASE("Вместимость ДК, созданного из некоторого множества элементов, больше или равен "
        "объёму этого ДК")
    {
        const auto t = burst::dynamic_tuple(std::vector<std::size_t>{}, 42, X{}, 3.14);
        CHECK(t.capacity() >= t.volume());
    }

    TEST_CASE("Резервирование меньшей вместимости, чем уже есть, не изменяет вместимость ДК")
    {
        auto t = burst::dynamic_tuple(true, 42, std::string("qwe"));
        auto old_capacity = t.capacity();

        t.reserve(t.capacity() / 2);

        CHECK(t.capacity() == old_capacity);
    }

    TEST_CASE("Резервирование вместимости, равной текущей, не изменяет вместимость ДК")
    {
        auto t = burst::dynamic_tuple(true, 42, std::string("qwe"));
        auto old_capacity = t.capacity();

        t.reserve(t.capacity());

        CHECK(t.capacity() == old_capacity);
    }

    TEST_CASE("Резервирование вместимости для пустого ДК делает его вместимость не меньше, "
        "чем было запрошено при вызове")
    {
        auto t = burst::dynamic_tuple{};

        t.reserve(10);

        CHECK(t.capacity() >= 10);
    }

    TEST_CASE("Запрос на резервирование большей вместимости для непустого ДК делает его "
        "вместимость не меньше, чем было запрошено при вызове")
    {
        auto t = burst::dynamic_tuple(false, 3.14, 42);

        auto desired_capacity = t.capacity() + 10;
        t.reserve(desired_capacity);

        CHECK(t.capacity() >= desired_capacity);
    }

    TEST_CASE("Запрос на резервирование большей вместимости для непустого ДК приводит к "
        "переносу внутренних объектов на новое место")
    {
        std::size_t move_count = 0;
        auto t =
            burst::dynamic_tuple
            (
                move_constructor_counter{move_count},
                move_constructor_counter{move_count}
            );

        auto move_count_before_reserve = move_count;

        auto desired_capacity = t.capacity() + 1;
        t.reserve(desired_capacity);

        CHECK(move_count == move_count_before_reserve + t.size());
    }

    TEST_CASE("Запрос на резервирование большей вместимости для непустого ДК приводит к "
        "разрушению старых объектов")
    {
        std::size_t destructor_count = 0;
        auto t =
            burst::dynamic_tuple
            (
                destructor_counter{destructor_count},
                destructor_counter{destructor_count}
            );

        auto destructor_count_before_reserve = destructor_count;

        auto desired_capacity = t.capacity() + 1;
        t.reserve(desired_capacity);

        CHECK(destructor_count == destructor_count_before_reserve + t.size());
    }

    TEST_CASE("Размер ДК равен количеству содержащихся в нём элементов")
    {
        const auto some_vector = std::vector<std::size_t>{};
        const auto some_integer = 42;
        const auto some_struct = X{};
        const auto some_floating = 3.14;

        burst::dynamic_tuple t(some_vector, some_integer, some_struct, some_floating);
        CHECK(t.size() == 4);
    }

    TEST_CASE("В конец ДК могут быть вставлены произвольные объекты")
    {
        burst::dynamic_tuple t;

        t.push_back(X{17, std::vector<int>{1, 2, 3}});
        t.push_back(true);
        t.push_back(std::string("123"));

        CHECK(t.size() == 3);
        CHECK(t.get<X>(0).a == 17);
        CHECK(t.get<X>(0).b == std::vector<int>{1, 2, 3});
        CHECK(t.get<bool>(1) == true);
        CHECK(t.get<std::string>(2) == std::string("123"));
    }

    TEST_CASE("Метод clear() уничтожает хранящиеся в ДК объекты")
    {
        burst::dynamic_tuple t;
        t.push_back(dummy{});

        REQUIRE(dummy::instances_count == 1);
        t.clear();

        CHECK(dummy::instances_count == 0);
    }

    TEST_CASE("Объём ДК равен нулю после вызова метода clear()")
    {
        burst::dynamic_tuple t(1, 3.14, true, std::string("qwe"));

        t.clear();

        CHECK(t.volume() == 0);
    }

    TEST_CASE("Размер ДК равен нулю после вызова метода clear()")
    {
        burst::dynamic_tuple t(1, 3.14, true, std::string("qwe"));

        t.clear();

        CHECK(t.size() == 0);
    }

    TEST_CASE("ДК пуст после вызова метода clear()")
    {
        burst::dynamic_tuple t(1, 3.14, true, std::string("qwe"));

        t.clear();

        CHECK(t.empty());
    }

    TEST_CASE("Вместимость ДК не изменяется после вызова метода clear()")
    {
        burst::dynamic_tuple t(1, 3.14, true, std::string("qwe"));
        const auto capacity = t.capacity();

        t.clear();

        CHECK(t.capacity() == capacity);
    }

    TEST_CASE("Исключение, брошенное вставляемым объектом, не приводит к утечкам")
    {
        REQUIRE(kamikaze::instances_count == 0);
        REQUIRE_THROWS_AS(burst::dynamic_tuple(kamikaze{}, kamikaze{}, kamikaze{}), std::runtime_error);
        CHECK(kamikaze::instances_count == 0);
    }

    TEST_CASE("Хрянящиеся в ДК объекты могут быть изменены извне")
    {
        burst::dynamic_tuple t(true, std::vector<char>{'a', 'b' ,'c'}, 10);

        auto & vector = t.get<std::vector<char>>(1);

        vector.push_back('q');
        CHECK(t.get<std::vector<char>>(1) == std::vector<char>{'a', 'b' ,'c', 'q'});

        vector.clear();
        CHECK(t.get<std::vector<char>>(1).empty());
    }

    TEST_CASE("Конструктор переноса ДК переносит элементы одного ДК в другой, а не вставляет "
        "один ДК внутрь другого ДК")
    {
        burst::dynamic_tuple moved(2.71, 3, false);
        burst::dynamic_tuple constructed(std::move(moved));
        CHECK(constructed.size() == 3);
    }

    TEST_CASE("Конструктор копирования ДК копирует элементы одного ДК в другой, а не вставляет "
        "один ДК внутрь другого ДК")
    {
        burst::dynamic_tuple copied(2.71, 3, false);
        burst::dynamic_tuple constructed(copied);
        CHECK(constructed.size() == 3);
    }

    TEST_CASE("ДК может содержать в себе ДК")
    {
        burst::dynamic_tuple t(burst::dynamic_tuple{1, 2}, burst::dynamic_tuple{3, 4});
        CHECK(t.size() == 2);
    }

    TEST_CASE("Хранящиеся в ДК объекты переносятся при переносе ДК")
    {
        REQUIRE(dummy::instances_count == 0);
        burst::dynamic_tuple one(dummy{});
        REQUIRE(dummy::instances_count == 1);

        burst::dynamic_tuple another(std::move(one));
        CHECK(dummy::instances_count == 1);

        one = std::move(another);
        CHECK(dummy::instances_count == 1);
    }

    struct throw_on_move
    {
        throw_on_move () = default;
        throw_on_move (const throw_on_move &) = default;
        throw_on_move & operator = (const throw_on_move &) = default;
        throw_on_move (throw_on_move &&)
        {
            throw std::runtime_error(u8"Пока!"_u8s);
        }
        throw_on_move & operator = (throw_on_move &&) = default;
        ~throw_on_move () = default;
    };

    TEST_CASE("Исключение при переносе внутренних объектов не приводит к утечкам")
    {
        const throw_on_move thrower{};

        REQUIRE(dummy::instances_count == 0);
        {
            burst::dynamic_tuple t;
            t.push_back(dummy{});
            t.push_back(thrower);
            REQUIRE(dummy::instances_count == 1);

            REQUIRE_THROWS_AS(t.reserve(t.capacity() + 1), std::runtime_error);
            CHECK(dummy::instances_count == 1);
        }
        CHECK(dummy::instances_count == 0);
    }

    TEST_CASE("Конструктор копирования ДК создаёт новые экземпляры хранящихся в копируемом ДК "
        "объектов и разрушает их при разрушении созданной копии ДК")
    {
        REQUIRE(dummy::instances_count == 0);
        {
            burst::dynamic_tuple initial(dummy{});

            REQUIRE(dummy::instances_count == 1);
            {
                burst::dynamic_tuple copy(initial);
                CHECK(dummy::instances_count == 2);
            }
            CHECK(dummy::instances_count == 1);
        }
        CHECK(dummy::instances_count == 0);
    }

    TEST_CASE("Оператор копирующего присвоения ДК создаёт новые экземпляры хранящихся в копируемом "
        "ДК объектов и разрушает их при разрушении этой копии ДК")
    {
        REQUIRE(dummy::instances_count == 0);
        {
            burst::dynamic_tuple initial(dummy{});

            REQUIRE(dummy::instances_count == 1);
            {
                burst::dynamic_tuple copy(dummy{});
                copy = initial;
                CHECK(dummy::instances_count == 2);
            }
            CHECK(dummy::instances_count == 1);
        }
        CHECK(dummy::instances_count == 0);
    }

    TEST_CASE("Объекты, хранящиеся в ДК, уничтожаются при копирующем присвоении")
    {
        REQUIRE(dummy::instances_count == 0);
        burst::dynamic_tuple initial(dummy{});
        REQUIRE(dummy::instances_count == 1);

        burst::dynamic_tuple other(1, 3.14, true);
        initial = other;
        CHECK(dummy::instances_count == 0);
    }

    TEST_CASE("Объекты, хранящиеся в ДК, уничтожаются при переносящем присвоении")
    {
        REQUIRE(dummy::instances_count == 0);
        burst::dynamic_tuple initial(dummy{});
        REQUIRE(dummy::instances_count == 1);

        burst::dynamic_tuple other(1, 3.14, true);
        initial = std::move(other);
        CHECK(dummy::instances_count == 0);
    }

    TEST_CASE("Конструктор переноса оставляет переносимый экземпляр ДК пустым")
    {
        burst::dynamic_tuple source(13, 3.14, true);
        burst::dynamic_tuple destination(std::move(source));
        CHECK(source.empty());
    }

    TEST_CASE("Оператор перенящего присвоения оставляет переносимый экземпляр ДК пустым")
    {
        burst::dynamic_tuple source(std::string("move me"));
        burst::dynamic_tuple destination(std::string("some items here"));
        destination = std::move(source);
        CHECK(source.empty());
    }

    TEST_CASE("Размер ДК-источника после переноса равен нулю")
    {
        burst::dynamic_tuple source(13, 3.14, true);
        burst::dynamic_tuple destination(std::move(source));
        CHECK(source.size() == 0);
    }

    TEST_CASE("Размер ДК-источника после переносящего присвоения равен нулю")
    {
        burst::dynamic_tuple source(std::string("move me"));
        burst::dynamic_tuple destination(std::string("some items here"));
        destination = std::move(source);
        CHECK(source.size() == 0);
    }

    TEST_CASE("Объём ДК-источника после переноса равен нулю")
    {
        burst::dynamic_tuple source(13, 3.14, true);
        burst::dynamic_tuple destination(std::move(source));
        CHECK(source.volume() == 0);
    }

    TEST_CASE("Объём ДК-источника после переносящего присвоения равен нулю")
    {
        burst::dynamic_tuple source(std::string("move me"));
        burst::dynamic_tuple destination(std::string("some items here"));
        destination = std::move(source);
        CHECK(source.volume() == 0);
    }

    TEST_CASE("Копирование ДК приводит к созданию настоящих копий хранящихся в нём объектов")
    {
        burst::dynamic_tuple initial(std::string("cat"));

        auto copy = burst::dynamic_tuple(initial);
        REQUIRE(copy.get<std::string>(0) == "cat");

        initial.get<std::string>(0).append("harsis");
        CHECK(copy.get<std::string>(0) == "cat");
    }

    TEST_CASE("Копирующее присвоение ДК приводит к созданию настоящих копий хранящихся в нём "
        "объектов")
    {
        burst::dynamic_tuple initial(std::vector<std::string>(1, "dog"));

        burst::dynamic_tuple copy(std::string("qwerty"));
        copy = initial;
        REQUIRE(copy.get<std::vector<std::string>>(0) == std::vector<std::string>(1, "dog"));

        initial.get<std::vector<std::string>>(0).push_back("horse");
        CHECK(copy.get<std::vector<std::string>>(0) == std::vector<std::string>(1, "dog"));
    }

    TEST_CASE("В результате копирующего присвоения объём копии равен объёму оригинала")
    {
        burst::dynamic_tuple initial(std::string("cat"), 5, 3.14, std::vector<int>{1, 2});

        burst::dynamic_tuple copy(true, 2.71);
        copy = initial;
        CHECK(initial.volume() == copy.volume());
    }

    TEST_CASE("В результате копирующего присвоения размер копии равен размеру оригинала")
    {
        burst::dynamic_tuple initial(std::string("cat"), 5, 3.14, std::vector<int>{1, 2});

        burst::dynamic_tuple copy(17);
        copy = initial;
        CHECK(initial.size() == copy.size());
    }

    TEST_CASE("Результат копирующего присвоения пустого ДК — пустой ДК")
    {
        burst::dynamic_tuple initial;

        burst::dynamic_tuple copy(std::string("123"), 'a');
        copy = initial;
        CHECK(copy.empty());
    }

    TEST_CASE("Порождает исключение при попытке скопировать некопируемый объект")
    {
        burst::dynamic_tuple initial(std::make_unique<int>(5));
        CHECK_THROWS_AS(burst::dynamic_tuple copy(initial), std::runtime_error);
    }

    TEST_CASE("Исключение при копировании хранящихся в ДК объектов не приводит к утечкам")
    {
        REQUIRE(dummy::instances_count == 0);
        {
            burst::dynamic_tuple t(dummy{}, std::make_unique<int>(17), dummy{});
            REQUIRE(dummy::instances_count == 2);

            REQUIRE_THROWS_AS(burst::dynamic_tuple copy(t), std::runtime_error);
            CHECK(dummy::instances_count == 2);
        }
        CHECK(dummy::instances_count == 0);
    }

    TEST_CASE("Исключение при копирующем присвоении хранящихся в ДК объектов не приводит к утечкам")
    {
        REQUIRE(dummy::instances_count == 0);
        {
            burst::dynamic_tuple t(dummy{}, std::make_unique<int>(17));
            burst::dynamic_tuple copy(dummy{});
            REQUIRE(dummy::instances_count == 2);

            REQUIRE_THROWS_AS(copy = t, std::runtime_error);
        }
        CHECK(dummy::instances_count == 0);
    }

    TEST_CASE("Переносящее присвоение ДК не приводит к утечкам")
    {
        REQUIRE(dummy::instances_count == 0);
        {
            burst::dynamic_tuple t(dummy{});
            REQUIRE(dummy::instances_count == 1);

            burst::dynamic_tuple copy(dummy{});
            REQUIRE(dummy::instances_count == 2);

            copy = std::move(t);
        }
        CHECK(dummy::instances_count == 0);
    }

    TEST_CASE("Умеет отдавать ссылки на неизменяемые значения по их отступу в ДК")
    {
        const burst::dynamic_tuple t(std::string("123"), 42, true);
        auto int_offset = t.offset(1);
        CHECK(t.get_by_offset<int>(int_offset) == 42);
    }

    TEST_CASE("Умеет отдавать ссылки на изменяемые значения по их отступу в ДК")
    {
        auto old_value = 42;
        auto new_value = 99;

        burst::dynamic_tuple t(std::string("123"), old_value, true);
        auto int_offset = t.offset(1);

        REQUIRE(t.get<int>(1) == old_value);
        t.get_by_offset<int>(int_offset) = new_value;

        CHECK(t.get<int>(1) == new_value);
    }

    TEST_CASE("Метод pop_back() уменьшает размер ДК на единицу")
    {
        auto t = burst::dynamic_tuple(true, 3.14, 42);

        t.pop_back();
        CHECK(t.size() == 2);
    }

    TEST_CASE("Объём ДК после вызова метода pop_back() равен объёму ДК, созданного из тех же "
        "самых объектов, что и исходный ДК, кроме последнего")
    {
        auto t = burst::dynamic_tuple(true, 42);
        t.pop_back();

        CHECK(t.volume() == burst::dynamic_tuple(true).volume());
    }

    TEST_CASE("У выброшенного методом pop_back() элемента вызывается деструктор")
    {
        auto t = burst::dynamic_tuple(std::string("qwerty"), 42, dummy{});
        REQUIRE(dummy::instances_count == 1);

        t.pop_back();
        CHECK(dummy::instances_count == 0);
    }

    TEST_CASE("Метод push_back() увеличивает размер ДК на единицу")
    {
        auto t = burst::dynamic_tuple{};

        t.push_back(1);
        CHECK(t.size() == 1);

        t.push_back(3.14);
        CHECK(t.size() == 2);
    }

    TEST_CASE("Метод push_back() увеличивает объём ДК на размер вставляемого объекта с учётом его "
        "выравнивания")
    {
        auto t = burst::dynamic_tuple{};

        using aligned_by_4 = struct alignas(4) {char a[4];};
        using aligned_by_8 = struct alignas(8) {char a[8];};

        t.push_back(aligned_by_4{});
        CHECK(t.volume() == 4);

        // Вставка элемента, выравнивание которого больше размера предыдущего элемента
        {
            t.push_back(aligned_by_8{});
            CHECK(t.volume() == 16);
        }

        // Вставка элемента, размер которого меньше текущего выравнивания
        {
            t.push_back('a');
            CHECK(t.volume() == 17);
        }
    }

    TEST_CASE("Метод pop_back() не изменяет вместимость ДК")
    {
        burst::dynamic_tuple t;
        for (auto i = 0; i < 100; ++i)
        {
            t.push_back(3.14);
        }

        auto old_capacity = t.capacity();

        while (not t.empty())
        {
            t.pop_back();
        }
        CHECK(t.capacity() == old_capacity);
    }

    TEST_CASE("Умеет отдавать std::type_info хранящегося внутри объекта по его индексу")
    {
        const auto t = burst::dynamic_tuple(std::string("123"));
        CHECK(t.type(0) == typeid(std::string));
    }

    TEST_CASE("Метод shrink_to_fit() уменьшает вместимость до объёма")
    {
        auto t = burst::dynamic_tuple(std::string("123"), 2.71, true);

        t.reserve(t.volume() * 2);
        t.shrink_to_fit();

        CHECK(t.capacity() == t.volume());
    }

    TEST_CASE("Метод shrink_to_fit() для пустого ДК уменьшает вместимость до нуля")
    {
        auto t = burst::dynamic_tuple{};

        t.reserve(100);
        t.shrink_to_fit();

        CHECK(t.capacity() == 0);
    }

    TEST_CASE("Метод shrink_to_fit() оставляет пустой ДК пустым")
    {
        auto t = burst::dynamic_tuple{};

        t.reserve(100);
        t.shrink_to_fit();

        CHECK(t.empty());
    }

    TEST_CASE("Метод shrink_to_fit() оставляет непустой ДК непустым")
    {
        auto t = burst::dynamic_tuple{1, false};

        t.reserve(t.volume() * 2);
        t.shrink_to_fit();

        CHECK(not t.empty());
    }

    TEST_CASE("Метод shrink_to_fit() не влияет на размер пустого ДК")
    {
        auto t = burst::dynamic_tuple{};

        t.reserve(100);
        t.shrink_to_fit();

        CHECK(t.size() == 0);
    }

    TEST_CASE("Метод shrink_to_fit() не влияет на размер непустого ДК")
    {
        auto t = burst::dynamic_tuple(1, true, 3.14);

        t.reserve(t.volume() * 2);
        t.shrink_to_fit();

        CHECK(t.size() == 3);
    }

    TEST_CASE("Метод shrink_to_fit() не влияет на объём пустого ДК")
    {
        auto t = burst::dynamic_tuple{};

        t.reserve(10);
        t.shrink_to_fit();

        CHECK(t.volume() == 0);
    }

    TEST_CASE("Метод shrink_to_fit() не влияет на объём непустого ДК")
    {
        auto t = burst::dynamic_tuple(1, true, 3.14);
        auto old_volume = t.volume();

        t.reserve(t.volume() * 2);
        t.shrink_to_fit();

        CHECK(t.volume() == old_volume);
    }

    TEST_CASE("Для ДК определена свободная функция swap()")
    {
        auto t = burst::dynamic_tuple(1, 3.14);
        auto u = burst::dynamic_tuple(std::string("qwe"), true);

        swap(t, u);
    }
}
