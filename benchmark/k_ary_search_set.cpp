#include <algorithm>
#include <iostream>
#include <set>
#include <unordered_set>
#include <vector>

#include <boost/container/flat_set.hpp>
#include <boost/program_options.hpp>

#include <thrust/container/k_ary_search_set.hpp>

template <typename Container>
struct default_constructor
{
    default_constructor (const std::string & name):
        name(name)
    {
    }

    template <typename Iterator>
    Container operator () (Iterator first, Iterator last) const
    {
        std::cout << name << std::endl;
        return Container(first, last);
    }

    std::string name;
};

template <typename Value>
struct k_ary_constructor
{
    k_ary_constructor (std::size_t arity):
        arity(arity)
    {
    }

    template <typename Iterator>
    thrust::k_ary_search_set<Value> operator () (Iterator first, Iterator last) const
    {
        std::cout << "k_ary_set(" << arity << ")" << std::endl;
        return thrust::k_ary_search_set<Value>(first, last, arity);
    }

    std::size_t arity;
};

template <typename Container>
void read (std::istream & stream, Container & values)
{
    for (typename Container::value_type n; stream >> n; /* пусто */)
    {
        values.push_back(n);
    }

    std::sort(values.begin(), values.end());
    values.erase(std::unique(values.begin(), values.end()), values.end());
}

template <typename Container, typename SetConstructor>
void test_one (const Container & numbers, std::size_t attempt_count, const SetConstructor & constructor)
{
    clock_t creation_time = clock();
    auto set = constructor(numbers.begin(), numbers.end());
    creation_time = clock() - creation_time;

    clock_t search_time = 0;
    for (std::size_t attempt = 0; attempt < attempt_count; ++attempt)
    {
        const auto & element = numbers[static_cast<std::size_t>(random()) % numbers.size()];

        clock_t attempt_time = clock();
        auto search_result = set.find(element);
        attempt_time = clock() - attempt_time;

        if (search_result == set.end() || *search_result != element)
        {
            throw std::runtime_error("Нашлись не все искомые элементы.");
        }

        search_time += attempt_time;
    }

    std::cout << "\tВремя создания: " << static_cast<double>(creation_time) / CLOCKS_PER_SEC << std::endl;
    std::cout << "\tСреднее время поиска: " << static_cast<double>(search_time) / attempt_count / CLOCKS_PER_SEC << std::endl;
    std::cout << std::endl;
}

template <typename Container>
void test (const Container & arities, std::size_t attempts)
{
    std::vector<std::int64_t> numbers;
    read(std::cin, numbers);

    for (auto arity: arities)
    {
        test_one(numbers, attempts, k_ary_constructor<std::int64_t>(arity));
    }

    test_one(numbers, attempts, default_constructor<std::set<std::int64_t>>("set"));
    test_one(numbers, attempts, default_constructor<boost::container::flat_set<std::int64_t>>("flat_set"));
    test_one(numbers, attempts, default_constructor<std::unordered_set<std::int64_t>>("hash_set"));
}

int main (int argc, const char * argv[])
{
    namespace bpo = boost::program_options;

    bpo::options_description description("Опции");
    description.add_options()
        ("help,h", "Подсказка")
        ("arity", bpo::value<std::vector<std::size_t>>()->multitoken(), "Набор кратностей для испытаний")
        ("attempts", bpo::value<std::size_t>()->default_value(1000));

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
            std::vector<std::size_t> arities = vm["arity"].as<std::vector<std::size_t>>();
            std::size_t attempts = vm["attempts"].as<std::size_t>();

            test(arities, attempts);
        }
    }
    catch (std::exception &)
    {
        std::cout << description << std::endl;
    }
}
