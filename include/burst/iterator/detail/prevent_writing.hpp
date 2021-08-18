#ifndef BURST__ITERATOR__DETAIL__PREVENT_WRITING_HPP
#define BURST__ITERATOR__DETAIL__PREVENT_WRITING_HPP

namespace burst
{
    namespace detail
    {
        template <typename T>
        struct prevent_writing
        {
            using type = T;
        };

        template <typename T>
        struct prevent_writing <T &>
        {
            using type = const T &;
        };

        template <typename T>
        using prevent_writing_t = typename prevent_writing<T>::type;
    }
}

#endif // BURST__ITERATOR__DETAIL__PREVENT_WRITING_HPP
