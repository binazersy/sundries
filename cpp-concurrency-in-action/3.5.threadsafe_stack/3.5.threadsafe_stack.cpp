/**=======================================================================

	3.5.threadsafe_stack.cpp

		@author: binazersy@gmail.com

=======================================================================**/


#include <iostream>
#include <thread>
#include <memory>
#include <exception>
#include <mutex>
#include <stack>


struct empty_stack : std::exception
{
	const char* what() const throw() { return "empty stack"; }
};


//!---------------------------------------------------------
//! @brief Threadsafe Stack class
//!---------------------------------------------------------
//! 
template<typename T>
class threadsafe_stack
{
private:
	std::stack<T> _data;
	mutable std::mutex _m;

public:
	threadsafe_stack() {}
	
	threadsafe_stack(const threadsafe_stack& other)
	{
		std::lock_guard<std::mutex> lock(other._m);
		_data = other._data;
	}

	threadsafe_stack& operator=(const threadsafe_stack&) = delete;

	void push(T value)
	{
		std::lock_guard<std::mutex> lock(_m);
		_data.push(value);
	}

	std::shared_ptr<T> pop()
	{
		std::lock_guard<std::mutex> lock(_m);
		if (_data.empty())
			throw empty_stack();

		std::shared_ptr<T> const res(std::make_shared<T>(_data.top()));
		_data.pop();
		return res;
	}

	void pop(T& value)
	{
		std::lock_guard<std::mutex> lock(_m);
		if (_data.empty())
			throw empty_stack();

		value = _data.top();
		_data.pop();
	}

	bool empty() const
	{
		std::lock_guard<std::mutex> lock(_m);
		return _data.empty();
	}
};


int main()
{
	threadsafe_stack<int> st;
	st.push(1);
	auto i = st.pop();

    return 0;
}

