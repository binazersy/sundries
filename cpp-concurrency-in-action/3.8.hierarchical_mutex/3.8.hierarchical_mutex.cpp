/**=======================================================================

	3.8.hierarchical_mutex.cpp

		@author: binazersy@gmail.com

=======================================================================**/


#include <mutex>


//!---------------------------------------------------------
//! @brief Hierarchical Mutex class
//!---------------------------------------------------------
//! 
class hierarchical_mutex
{
	std::mutex _mutex;
	unsigned long const _hierarchy_value;
	unsigned long _previous_hierarchy_value;
	static thread_local unsigned long _this_thread_hierarchy_value;

	void check_for_hierarchy_violation()
	{
		if (_this_thread_hierarchy_value <= _hierarchy_value)
		{
			throw std::logic_error("mutex hierarchy violated");
		}
	}

	void update_hierarchy_value()
	{
		_previous_hierarchy_value = _this_thread_hierarchy_value;
		_this_thread_hierarchy_value = _hierarchy_value;
	}

public:
	explicit hierarchical_mutex(unsigned long value)
		: _hierarchy_value(value), _previous_hierarchy_value(0) {}

	void lock()
	{
		check_for_hierarchy_violation();
		_mutex.lock();
		update_hierarchy_value();
	}

	void unlock()
	{
		_this_thread_hierarchy_value = _previous_hierarchy_value;
		_mutex.unlock();
	}

	bool trylock()
	{
		check_for_hierarchy_violation();
		if (!_mutex.try_lock())
			return false;
		update_hierarchy_value();
		return true;
	}
};


int main()
{
	// no test code ^^;

	return 0;
}