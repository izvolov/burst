#include <burst/algorithm/radix_sort/radix_sort_seq.hpp>

#include <cassert>
#include <string>
#include <vector>

int main ()
{
    std::vector<std::string> strings{"aaaa", "bbb", "cc", "d"};

    std::vector<std::string> buffer(strings.size());
    burst::radix_sort(strings.begin(), strings.end(), buffer.begin(),
        [] (const std::string & string)
        {
            return string.size(); // Строки сортируются относительно их длины.
        }
    );
    assert((strings == std::vector<std::string>{"d", "cc", "bbb", "aaaa"}));
}
