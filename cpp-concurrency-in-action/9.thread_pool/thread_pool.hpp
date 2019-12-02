#pragma once


#include <thread>
#include <atomic>
#include <future>
#include <algorithm>
#include <numeric>
#include <list>
#include "queue.hpp"


class join_threads
{
	std::vector<std::thread>& _threads;
public:
	explicit join_threads(std::vector<std::thread>& r) : _threads(r) {}
	~join_threads()
	{
		for (unsigned long i = 0; i<_threads.size(); ++i)
		{
			if (_threads[i].joinable())
				_threads[i].join();
		}
	}
};


class thread_pool_simple
{
	std::atomic_bool							_done;
	threadsafe_queue<std::function<void()> >	_queue;
	std::vector<std::thread>					_threads;
	join_threads								_joiner;

	void worker_thread()
	{
		while (!_done)
		{
			std::function<void()> task;
			if (_queue.try_pop(task))
			{
				task();
			}
			else
			{
				std::this_thread::yield();
			}
		}
	}

public:
	thread_pool_simple() : _done(false), _joiner(_threads)
	{
		const unsigned thread_count = std::thread::hardware_concurrency();
		try
		{
			for (unsigned i = 0; i < thread_count; ++i)
			{
				_threads.push_back(std::thread(&thread_pool_simple::worker_thread, this));
			}
		}
		catch (...)
		{
			_done = true;
			throw;
		}
	}

	~thread_pool_simple()
	{
		_done = true;
	}

	template<typename FunctionType> void submit(FunctionType f)
	{
		_queue.push(std::function<void()>(f));
	}
};


class function_wrapper
{
	struct impl_base
	{
		virtual void call() = 0;
		virtual ~impl_base() {}
	};

	template<typename F>
	struct impl_type : impl_base
	{
		F func;
		impl_type(F&& f) : func(std::move(f)) {}
		void call() { func(); }
	};

	std::unique_ptr<impl_base>	_impl;

public:
	template<typename F> function_wrapper(F&& f)
		: _impl(new impl_type<F>(std::move(f))) {}
	function_wrapper() = default;
	function_wrapper(function_wrapper&& other) : _impl(std::move(other._impl)) {}

	void operator()() { _impl->call(); }
	function_wrapper& operator=(function_wrapper&& other)
	{
		_impl = std::move(other._impl);
		return *this;
	}

	function_wrapper(const function_wrapper&) = delete;
	//function_wrapper(function_wrapper&) = delete;
	function_wrapper& operator=(const function_wrapper&) = delete;
};


class thread_pool
{
	std::atomic_bool					_done;
	threadsafe_queue<function_wrapper>	_queue;
	std::vector<std::thread>			_threads;
	join_threads						_joiner;

	// local queue.
	// !!! BUT this yield push/pop operations goes not real queue operations.
	typedef std::queue<function_wrapper> local_queue_type;
	static thread_local std::unique_ptr<local_queue_type>	_local_queue;
	std::atomic_bool					_use_local;

	void worker_thread()
	{
		if (_use_local)
			_local_queue.reset(new local_queue_type);

		while (!_done)
		{
			run_pending_task();
		}
	}

public:
	thread_pool(bool use_local = false)
		: _done(false), _joiner(_threads), _use_local(use_local)
	{
		const unsigned thread_count = std::thread::hardware_concurrency();
		try
		{
			for (unsigned i = 0; i < thread_count; ++i)
			{
				_threads.push_back(std::thread(&thread_pool::worker_thread, this));
			}
		}
		catch (...)
		{
			_done = true;
			throw;
		}
	}

	~thread_pool()
	{
		_done = true;
	}

	template<typename FunctionType>
	std::future<typename std::result_of<FunctionType ()>::type>
		submit(FunctionType f)
	{
		typedef typename std::result_of<FunctionType ()>::type result_type;
		std::packaged_task<result_type ()> task(std::move(f));
		std::future<result_type> res(task.get_future());
		if (_local_queue)
		{
			_local_queue->push(std::move(task));
		}
		else
		{
			_queue.push(std::move(task));
		}
		return res;
	}

	void run_pending_task()
	{
		function_wrapper task;
		if (_local_queue && !_local_queue->empty())
		{
			task = std::move(_local_queue->front());
			_local_queue->pop();
			task();
		}
		else if (_queue.try_pop(task))
		{
			task();
		}
		else
		{
			std::this_thread::yield();
		}
	}
};

thread_local std::unique_ptr<std::queue<function_wrapper> >	thread_pool::_local_queue;


template<typename Iterator, typename T>
struct accumulate_block
{
	T operator() (Iterator first, Iterator last)
	{
		return std::accumulate(first, last, T());
	}

	T accumulate(Iterator first, Iterator last)
	{
		return std::accumulate(first, last, T());
	}
};


template<typename Iterator, typename T>
T parallel_accmulate_using_tp(Iterator first, Iterator last, T init)
{
	const unsigned long length = std::distance(first, last);
	if (!length)
		return init;

	const unsigned long block_size = 25;
	const unsigned long num_blocks = (length + block_size - 1) / block_size;

	std::vector<std::future<T> > futures(num_blocks - 1);
	thread_pool pool;

	Iterator block_start = first;
	for (unsigned long i = 0; i < (num_blocks - 1); ++i)
	{
		Iterator block_end = block_start;
		std::advance(block_start, block_size);

		futures[i] = pool.submit(std::bind(accumulate_block<Iterator, T>(), block_start, block_end));
		block_start = block_end;
	}

	T last_result = accumulate_block<Iterator, T>() (block_start, last);
	T result = init;
	for (unsigned long i = 0; i < (num_blocks - 1); ++i)
	{
		result += futures[i].get();
	}
	result += last_result;
	return result;
}


template<typename T>
struct sorter_using_tp
{
	thread_pool pool;

	std::list<T> do_sort(std::list<T>& chunk)
	{
		if (chunk.empty())
			return chunk;

		std::list<T> result;
		result.splice(result.begin(), chunk, chunk.begin());	// move 1st element chunk to result
		const T& partition_value = *result.begin();		// pivot

		// calc. divide point
		typename std::list<T>::iterator divide_point =
			std::partition(chunk.begin(), chunk.end(), [&](T const& val) { return val < partition_value; });
		
		// lower
		std::list<T> new_lower_chunk;
		new_lower_chunk.splice(new_lower_chunk.end(), chunk, chunk.begin(), divide_point);
		
		std::future<std::list<T> > new_lower =
			pool.submit(std::bind(&sorter_using_tp::do_sort, this, std::move(new_lower_chunk)));

		// higher
		std::list<T> new_higher{ do_sort(chunk) };

		result.splice(result.end(), new_higher);
		//while (!new_lower.wait_for(std::chrono::seconds(0)) == std::future_status::timeout)
		while (std::future_status::timeout == new_lower.wait_for(std::chrono::seconds(0)))
		{
			pool.run_pending_task();
		}

		result.splice(result.begin(), new_lower.get());

		return result;
	}
};


template<typename T>
std::list<T> parallel_quick_sort_using_tp(std::list<T> input)
{
	if (input.empty())
		return input;

	sorter_using_tp<T> s;
	return s.do_sort(input);
}