/**=======================================================================

	threadsafe_stack.hpp

		@author: binazersy@gmail.com

		Again, from chapter 3.

=======================================================================**/


#include <stack>
#include <mutex>
#include <exception>


struct empty_stack : std::exception
{
	const char* what() const throw() { return "empty stack"; }
};


template <typename T>
class threadsafe_stack
{
private:
	std::stack<T>	_data;
	mutable std::mutex		_mutex;

public:
	threadsafe_stack() {}
	threadsafe_stack(const threadsafe_stack& other)
	{
		std::lock_guard<std::mutex> lock(other._mutex);
		_data = other._data;
	}

	threadsafe_stack& operator=(const threadsafe_stack &) = delete;

	void push(T value)
	{
		std::lock_guard<std::mutex> lock(_mutex);
		_data.push(std::move(value));
	}

	std::shared_ptr<T> pop()
	{
		std::lock_guard<std::mutex> lock(_mutex);
		if (_data.empty())
			throw empty_stack();
		
		std::shared_ptr<T> ptr = std::make_shared<T>(std::move(_data.top()));
		_data.pop();
		return ptr;
	}

	void pop(T& value)
	{
		std::lock_guard<std::mutex> lock(_mutex);
		if (_data.empty())
			throw empty_stack();

		value = std::move(_data.top());
		_data.pop();
	}

	bool empty() const
	{
		std::lock_guard<std::mutex> lock(_mutex);
		return _data.empty();
	}
};