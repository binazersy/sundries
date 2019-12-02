/**=======================================================================

	8.4.parallel_accumulate.cpp

		@author: binazersy@gmail.com

=======================================================================**/


#include <thread>
#include <future>
#include <vector>
#include <algorithm>
#include <iostream>
#include <numeric>


template<typename Iterator, typename T>
struct accumulate_block
{
	T operator() (Iterator first, Iterator last)
	{
		return std::accumulate(first, last, T());
	}
};


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


template<typename Iterator, typename T>
T parallel_accumulate(Iterator first, Iterator last, T init)
{
	const unsigned long length = std::distance(first, last);
	if (!length)
		return init;

	const unsigned long min_per_thread = 25;
	const unsigned long max_threads = (length + min_per_thread - 1) / min_per_thread;
	const unsigned long hardware_threads = std::thread::hardware_concurrency();
	// determine no. of threads, min( hardware thread, max thread by length )
	const unsigned long num_threads = std::min(0 != hardware_threads ? hardware_threads : 2, max_threads);
	const unsigned long block_size = length / num_threads;

	std::vector<std::future<T> > futures(num_threads - 1);	// one(last) for this thread
	std::vector<std::thread> threads(num_threads - 1);
	join_threads joiner(threads);

	Iterator block_start = first;
	for (unsigned long i = 0; i < (num_threads - 1); ++i)
	{
		Iterator block_end = block_start;
		std::advance(block_end, block_size);

		std::packaged_task<T(Iterator, Iterator)> task{ accumulate_block<Iterator, T>() };
		futures[i] = task.get_future();
		threads[i] = std::thread(std::move(task), block_start, block_end);
		block_start = block_end;
	}

	T last_result = accumulate_block<Iterator, T>()(block_start, last);		// last for this thread

	// replaced with join_threads. and then future::get() will block until result ready
	// for join only, could use finally block
	// but exception captured by futures may be lost if you do not rethrow it;
	//std::for_each(threads.begin(), threads.end(), std::mem_fn(&std::thread::join));

	T result = init;
	for (unsigned long i = 0; i < (num_threads - 1); ++i)
	{
		result += futures[i].get();
	}
	result += last_result;
	return result;
}


int main()
{
	std::vector<long> values;

	for (long i = 1; i < 100000; ++i)
	{
		values.push_back(i);
	}

	long total = parallel_accumulate(values.begin(), values.end(), (long)0);

	std::cout << "Total: " << total << std::endl;


	getchar();

	return 0;
}

