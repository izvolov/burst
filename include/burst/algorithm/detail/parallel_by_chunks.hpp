#ifndef BURST__ALGORITHM__DETAIL__PARALLEL_BY_CHUNKS_HPP
#define BURST__ALGORITHM__DETAIL__PARALLEL_BY_CHUNKS_HPP

#include <burst/type_traits/invoke_result.hpp>
#include <burst/type_traits/iterator_difference.hpp>

#include <boost/asio/post.hpp>
#include <boost/asio/thread_pool.hpp>

#include <algorithm>
#include <cstddef>
#include <future>
#include <iterator>
#include <utility>
#include <vector>

namespace burst
{
    namespace detail
    {
        template <typename Executor, typename NullaryFunction>
        auto post (Executor & e, NullaryFunction && f)
        {
            using return_type = invoke_result_t<NullaryFunction>;
            auto task = std::packaged_task<return_type ()>(std::forward<NullaryFunction>(f));
            auto future = task.get_future();

            boost::asio::post(e, std::move(task));

            return future;
        }

        // TODO: https://github.com/izvolov/burst/issues/150
        template <typename RandomAccessIterator, typename TernaryFunction>
        iterator_difference_t<RandomAccessIterator>
            parallel_by_chunks
            (
                boost::asio::thread_pool & pool,
                iterator_difference_t<RandomAccessIterator> chunk_size,
                RandomAccessIterator first,
                RandomAccessIterator last,
                TernaryFunction process_chunk
            )
        {
            std::vector<std::future<void>> results;

            auto chunk_count = iterator_difference_t<RandomAccessIterator>{0};
            while (first != last)
            {
                using std::distance;
                auto chunk_begin = first;
                auto chunk_end =
                    std::next(chunk_begin, std::min(distance(chunk_begin, last), chunk_size));

                results.push_back(detail::post(pool,
                    [process_chunk, chunk_count, chunk_begin, chunk_end]
                    {
                        process_chunk(chunk_count, chunk_begin, chunk_end);
                    }));

                first = chunk_end;
                ++chunk_count;
            }

            std::for_each(results.begin(), results.end(), [] (auto & r) {r.get();});

            return chunk_count;
        }
    } // namespace detail
} // namespace burst

#endif // BURST__ALGORITHM__DETAIL__PARALLEL_BY_CHUNKS_HPP
