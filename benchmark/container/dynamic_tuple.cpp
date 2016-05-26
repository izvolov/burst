#include <burst/container/dynamic_tuple.hpp>

#include <boost/program_options.hpp>

#include <algorithm>
#include <chrono>
#include <cstddef>
#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <random>
#include <utility>
#include <vector>

struct A
{
    std::size_t x = 0;
};

template <typename T>
burst::dynamic_tuple create_dynamic_tuple (std::size_t size)
{
    burst::dynamic_tuple t;

    for (std::size_t i = 0; i < size; ++i)
    {
        t.push_back(T{});
    }

    return t;
}

template <typename T>
auto create_pointer_array (std::size_t size)
{
    std::vector<std::unique_ptr<A>> pointer_array;

    for (std::size_t i = 0; i < size; ++i)
    {
        pointer_array.push_back(std::make_unique<A>());
    }

    return pointer_array;
}

template <typename T, typename URNG>
auto create_shuffled_pointer_array (std::size_t size, std::size_t spread, URNG && generator)
{
    std::vector<std::unique_ptr<A>> pointer_array = create_pointer_array<T>(size * spread);

    std::shuffle(pointer_array.begin(), pointer_array.end(), std::forward<URNG>(generator));
    pointer_array.resize(size);

    return pointer_array;
}

template <typename T>
auto create_sparse_pointer_array (std::size_t size, std::size_t spread)
{
    std::vector<std::unique_ptr<A>> pointer_array = create_pointer_array<T>(size * spread);

    auto last =
        std::remove_if(pointer_array.begin(), pointer_array.end(),
            [spread, index = 0ul] (const auto &) mutable
            {
                return index++ % spread != 0;
            });
    pointer_array.erase(last, pointer_array.end());
    assert(pointer_array.size() == size);

    return pointer_array;
}

template <typename Container, typename Range, typename Access>
double test_consecutive_access (Container container, Range && positions, Access access, std::size_t attempt_count)
{
    using namespace std::chrono;

    std::size_t total = 0;

    auto start_time = steady_clock::now();

    for (std::size_t attempt = 0; attempt < attempt_count; ++attempt)
    {
        for (auto position: positions)
        {
            total += access(container, position);
        }
    }

    auto total_time = steady_clock::now() - start_time;

    std::clog << total << std::endl;
    return duration_cast<duration<double>>(total_time).count();
}

void test_dyntuple_access (std::size_t size, std::size_t attempt_count)
{
    auto tuple = create_dynamic_tuple<A>(size);

    std::vector<std::size_t> offsets(size);
    std::iota(offsets.begin(), offsets.end(), 0);
    std::transform(offsets.begin(), offsets.end(), offsets.begin(),
        [& tuple] (std::size_t index)
        {
            return tuple.offset_of(index);
        });

    auto access = [] (const auto & t, std::size_t offset) {return t.template get_by_offset<A>(offset).x;};
    auto time = test_consecutive_access(tuple, offsets, access, attempt_count);
    std::cout << "Доступ в ДК: " << ' ' << time << std::endl;
}

void test_pointer_array_access
    (
        std::size_t size,
        std::size_t attempt_count,
        const std::string & type,
        std::size_t spread
    )
{
    using create_tuple_type = std::function<std::vector<std::unique_ptr<A>> ()>;
    static const std::map<std::string, create_tuple_type> create
    {
        {"plain", [size]
            {return create_pointer_array<A>(size);}},
        {"sparse", [size, spread]
            {return create_sparse_pointer_array<A>(size, spread);}},
        {"shuffled", [size, spread]
            {return create_shuffled_pointer_array<A>(size, spread, std::default_random_engine{});}}
    };

    auto pointer_array = create.at(type)();

    std::vector<std::size_t> indices(size);
    std::iota(indices.begin(), indices.end(), 0);

    auto access = [] (const auto & a, std::size_t index) {return a[index]->x;};
    auto time = test_consecutive_access(std::move(pointer_array), indices, access, attempt_count);
    std::cout << "Доступ в МУ: " << ' ' << time << std::endl;
}

int main (int argc, const char * argv[])
{
    namespace bpo = boost::program_options;

    bpo::options_description description("Опции");
    description.add_options()
        ("help,h", "Подсказка.")
        ("size", bpo::value<std::size_t>()->default_value(1000),
            "Размер испытываемых массивов.")
        ("attempts", bpo::value<std::size_t>()->default_value(1000),
            "Количество испытаний.")
        ("type", bpo::value<std::string>()->default_value("sparse"),
            "Тип массива указателей.\nДопустимые значения: plain, sparse, shuffled.")
        ("spread", bpo::value<std::size_t>()->default_value(1),
            "Степень разреженности массива указателей.\n"
            "Создаёт массив указателей размера `size * spread`, который затем, в зависимости от "
            "выбранного типа массива, либо прореживается, либо перемешивается.\n"
            "В случае с простым (plain) массивом опция игнорируется.");

    try
    {
        bpo::variables_map vm;
        bpo::store(bpo::parse_command_line(argc, argv, description), vm);
        bpo::notify(vm);

        if (vm.count("help"))
        {
            std::cout << description << std::endl;
        }
        else
        {
            auto size = vm["size"].as<std::size_t>();
            auto attempts = vm["attempts"].as<std::size_t>();
            auto type = vm["type"].as<std::string>();
            auto spread = vm["spread"].as<std::size_t>();

            test_dyntuple_access(size, attempts);
            test_pointer_array_access(size, attempts, type, spread);
        }
    }
    catch (bpo::error & e)
    {
        std::cout << e.what() << std::endl;
        std::cout << description << std::endl;
    }
}
