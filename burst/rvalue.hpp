#ifndef BURST_RVALUE_HPP
#define BURST_RVALUE_HPP

#include <type_traits>

namespace burst
{
    //!     Строгая ссылка на rvalue
    /*!
            Позволяет удобно описывать функции, которые в качестве аргумента получают произвольный
        объект и принимают на себя владение этим объектом.
            В отличие от варианта с принятием объекта по значению, позволяет не вызывать лишний раз
        конструктор переноса.

            Пример использования:

                template <typename T>
                void f (rvalue<T> t)
                {
                    g(std::move(t)); // Можно не бояться переносить.
                }

                int main ()
                {
                    f(17); // Работает.

                    auto x = 4;
                    f(x); // Ошибка компиляции.
                }
     */
    template <typename T,
        typename = std::enable_if_t<std::is_rvalue_reference<T &&>::value>>
    using rvalue = T &&;
}

#endif // BURST_RVALUE_HPP
