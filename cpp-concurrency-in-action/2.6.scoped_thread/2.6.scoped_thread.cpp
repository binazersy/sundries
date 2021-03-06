/**=======================================================================

	2.6.scoped_thread.cpp

		@author: binazersy@gmail.com

=======================================================================**/


#include <iostream>
#include <thread>


//!---------------------------------------------------------
//! @brief Scoped Thread class
//!---------------------------------------------------------
//! 
class scoped_thread
{
	std::thread _t;

public:
	scoped_thread(std::thread t)
		: _t(std::move(t))
	{
		if (!_t.joinable())
			throw std::logic_error("No Thread");
	}

	~scoped_thread()
	{
		//if (_t.joinable())		// alredy in constructor
			_t.join();
	}

	scoped_thread(scoped_thread const&) = delete;
	scoped_thread& operator=(const scoped_thread&) = delete;
};


struct func
{
	int& _i;
	func(int& i) : _i(i) {}
	void operator()()
	{
		for (unsigned int i = _i; i < 1000; ++i)
		{
			std::cout << i << "\n";
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}
	}
};


int main()
{
	int some_local_state = 0;
	scoped_thread t{ std::thread(func(some_local_state)) };


	getchar();
    return 0;
}

