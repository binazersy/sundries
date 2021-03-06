/**=======================================================================

	2.2.raii.cpp

		@author: binazersy@gmail.com

=======================================================================**/


#include <iostream>
#include <thread>


//!---------------------------------------------------------
//! @brief Thread Guard class
//!---------------------------------------------------------
//! 
class thread_guard
{
	std::thread&	_thread;

public:
	explicit thread_guard(std::thread &t)
		: _thread(t)
	{
	}

	~thread_guard()
	{
		if (_thread.joinable())
			_thread.join();
	}

	thread_guard(thread_guard const&) = delete;
	thread_guard& operator=(thread_guard const&) = delete;
};


struct func
{
	int& _i;
	func(int& i) : _i(i) {}
	void operator()()
	{
		for (unsigned int i = _i; i < 10000; ++i)
		{
			std::cout << i;
		}
	}
};


int main()
{
	int some_local_state = 0;

	func my_func(some_local_state);

	std::thread t(my_func);

	thread_guard tg(t);

	std::cout << "i = " << my_func._i;

	getchar();

    return 0;
}

