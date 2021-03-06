/**=======================================================================

	4.1.condition_variable.cpp

		@author: binazersy@gmail.com

=======================================================================**/


#include <iostream>
#include <thread>
#include <condition_variable>
#include <queue>


const int TO_DO = 100;
int g_done = 0;
int g_i = 0;
int g_j = 0;


struct data_chunk
{
	int i;
	int j;
};


std::mutex mutex;
std::queue<data_chunk> data_queue;
std::condition_variable data_cond;


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
		std::lock_guard<std::mutex> lock(mutex);
		data_queue.push(data);
		data_cond.notify_one();
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
		std::unique_lock<std::mutex> lock(mutex);		// need lock & unlock while wait()
		data_cond.wait(lock, [] { return !data_queue.empty(); });

		data_chunk data = data_queue.front();
		data_queue.pop();

		lock.unlock();

		process(data);

		if (is_last_chunk(data))
			break;
	}
}


int main()
{
	std::thread process(data_processing_thread);
	std::thread prepare(data_preparation_thread);

	prepare.join();
	process.join();

	getchar();

    return 0;
}

