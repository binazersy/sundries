/**=======================================================================

	8.7.parallel_for_each.cpp

		@author: binazersy@gmail.com

=======================================================================**/


#include <iostream>
#include <thread>
#include <vector>
#include <algorithm>
#include <numeric>
#include <future>
#include <stdexcpt.h>


class join_threads
{
	std::vector<std::thread>& _threads;
public:
	explicit join_threads(std::vector<std::thread>& r) : _threads(r) {}
	~join_threads()
	{
		for (unsigned long i = 0; i < _threads.size(); ++i)
		{
			if (_threads[i].joinable())
				_threads[i].join();
		}
	}
};


//!---------------------------------------------------------
//! @brief Parallel version of for_each
//!---------------------------------------------------------
//! 
template<typename Iterator, typename Func>
void parallel_for_each(Iterator first, Iterator last, Func f)
{
	unsigned long const length = std::distance(first, last);
	if (0 == length)
		return;

	const unsigned long min_per_thread = 25;
	const unsigned long max_threads = (length + min_per_thread - 1) / min_per_thread;
	const unsigned long hardware_threads = std::thread::hardware_concurrency();
	// determine no. of threads, min( hardware thread, max thread by length )
	const unsigned long num_threads = std::min(0 != hardware_threads ? hardware_threads : 2, max_threads);
	const unsigned long block_size = length / num_threads;

	std::vector<std::future<void> > futures(num_threads - 1);	// one(last) for this thread
	std::vector<std::thread> threads(num_threads - 1);
	join_threads joiner(threads);

	Iterator block_start = first;
	for (unsigned long i = 0; i < (num_threads - 1); ++i)
	{
		Iterator block_end = block_start;
		std::advance(block_end, block_size);

		std::packaged_task<void(void) > task(
			[=]() { std::for_each(block_start, block_end, f); }
		);
		futures[i] = task.get_future();
		threads[i] = std::thread(std::move(task));
		block_start = block_end;
	}

	std::for_each(block_start, last, f);

	for (unsigned long i = 0; i < (num_threads - 1); ++i)
	{
		futures[i].get();
	}
}


int main()
{
	std::vector<long> values(100000);

	int i = 0;
	for (auto& x : values)
	{
		x = ++i;
	}

	try
	{
		int total = std::accumulate(values.begin(), values.end(), 0);

		parallel_for_each(values.begin(), values.end(), [&](long &v) { v = v * 2; });

		int total_v = std::accumulate(values.begin(), values.end(), 0);

		std::cout << "Total: " << total << " (" << total_v << ")" << std::endl;
	}
	catch (std::exception &e)
	{
		std::cout << e.what() << std::endl;
	}

	getchar();

	return 0;
}

