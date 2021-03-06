/**=======================================================================

	4.5.threadsafe_queue.cpp

		@author: binazersy@gmail.com

=======================================================================**/


#include <queue>
#include <memory>
#include <mutex>
#include <condition_variable>
#include <iostream>
#include <list>


//!---------------------------------------------------------
//! @brief Threadsafe Queue class
//!---------------------------------------------------------
//! 
template<typename T>
class threadsafe_queue
{
private:
	mutable std::mutex _mutex;
	std::queue<T> _queue;
	std::condition_variable _cond;

public:
	threadsafe_queue() {}
	threadsafe_queue(const threadsafe_queue& other)
	{
		std::lock_guard<std::mutex> lock(other._mutex);
		_queue = other._queue;
	}

	threadsafe_queue& operator=(const threadsafe_queue&) = delete;

	void push(T value)
	{
		std::lock_guard<std::mutex> lock(_mutex);
		_queue.push(value);
		_cond.notify_one();		// to trigger multiple thread, use notify_all()
	}

	bool try_pop(T& value)
	{
		std::lock_guard<std::mutex> lock(_mutex);
		if (_queue.empty())
			return false;

		value = _queue.front();
		return true;
	}

	std::shared_ptr<T> try_pop()
	{
		std::lock_guard<std::mutex> lock(_mutex);
		if (_queue.empty())
			return std::shared_ptr<T>();

		std::shared_ptr<T> ptr{ std::make_shared<T>(_queue.front()) };
		_queue.pop();
		return ptr;
	}

	void wait_and_pop(T& value)
	{
		std::unique_lock<std::mutex> lock(_mutex);
		_cond.wait(lock, [this] { return !_queue.empty(); });

		value = _queue.front();
		_queue.pop();
	}

	std::shared_ptr<T> wait_and_pop()
	{
		std::unique_lock<std::mutex> lock(_mutex);
		_cond.wait(lock, [this] { return !_queue.empty(); });

		std::shared_ptr<T> ptr{ std::make_shared<T>(_queue.front()) };
		_queue.pop();
		return ptr;
	}

	bool empty() const
	{
		std::lock_guard<std::mutex> lock(_mutex);
		return _queue.empty();
	}
};


const int TO_DO = 100;
int g_done = 0;
int g_i = 0;
int g_j = 0;


struct data_chunk
{
	int i;
	int j;
};


threadsafe_queue<data_chunk> data_queue;


bool more_data_to_prepare()
{
	if (g_done >= TO_DO)
		return false;

	std::this_thread::sleep_for(std::chrono::milliseconds(100));
	return true;
}


data_chunk prepare_data()
{
	data_chunk data;
	data.i = ++g_i;
	data.j = ++g_j;
	return data;
}


void data_preparation_thread()
{
	while (more_data_to_prepare())
	{
		data_chunk const data = prepare_data();
		data_queue.push(data);
	}
}


void process(data_chunk& data)
{
	++g_done;
	std::cout << "Done: i = " << data.i << ", j = " << data.j << "\n";
}


bool is_last_chunk(data_chunk& data)
{
	return (g_done >= TO_DO) ? true : false;
}


void data_processing_thread()
{
	while (true)
	{
		data_chunk data;
		data_queue.wait_and_pop(data);
		process(data);
		if (is_last_chunk(data))
			break;
	}
}


int main()
{
	// this time, reverse creation and sleep for a while
	std::thread prepare(data_preparation_thread);
	std::this_thread::sleep_for(std::chrono::milliseconds(100));
	std::thread process(data_processing_thread);

	prepare.join();
	process.join();

	getchar();

	return 0;
}

