#pragma once

#include <queue>
#include <mutex>
#include <condition_variable>
#include <exception>


template<typename T>
class threadsafe_queue_old
{
private:
	std::queue<std::shared_ptr<T>>	_data;
	mutable std::mutex				_mutex;
	std::condition_variable			_cond;

public:
	threadsafe_queue_old() {}

	void wait_and_pop(T& value)
	{
		std::unique_lock<std::mutex> lock(_mutex);
		_cond.wait(lock, [this] { return !_data.empty(); });
		value = std::move(*_data.front());
		_data.pop();
	}

	bool try_pop(T& value)
	{
		std::lock_guard<std::mutex> lock(_mutex);
		if (_data.empty())
			return false;
		value = std::move(*_data.front());
		_data.pop();
	}

	std::shared_ptr<T> wait_and_pop()
	{
		std::unique_lock<std::mutex> lock(_mutex);
		_cond.wait(lock, [this] { return !_data.empty(); });
		std::shared_ptr<T> ptr = _data.front();
		_data.pop();
		return ptr;
	}

	std::shared_ptr<T> try_pop()
	{
		std::lock_guard<std::mutex> lock(_mutex);
		if (_data.empty())
			return std::shared_ptr<T>();

		std::shared_ptr<T> ptr = _data.front();
		_data.pop();
		return ptr;
	}

	void push(T value)
	{
		std::shared_ptr<T> ptr(std::make_shared<T>(std::move(value)));
		std::lock_guard<std::mutex> lock(_mutex);
		_data.push(ptr);
		_cond.notify_one();
	}

	bool empty() const
	{
		std::lock_guard<std::mutex> lock(_mutex);
		return _data.empty();
	}
};


//
//	simple single threaded queue using list
//
template<typename T>
class simple_queue
{
private:
	struct node
	{
		T data;
		std::unique_ptr<node> next;

		node(T d) : data(std::move(d)) {}
	};

	std::unique_ptr<node> _head;
	node* _tail;

public:
	simple_queue() {}

	simple_queue& operator=(const simple_queue&) = delete;

	std::shared_ptr<T> try_pop()
	{
		if (!_head)
			return std::shared_ptr<T>();

		std::shared_ptr<T> ptr{ std::make_shared<T>(std::move(_head->data)) };
		std::unique_ptr<node> const old = std::move(_head);
		_head = std::move(old->next);
		return ptr;
	}

	void push(T value)
	{
		std::unique_ptr<node> p{ new node(std::move(value)) };
		node* const new_tail = p.get();
		if (_tail)
		{
			_tail->next = std::move(p);
		}
		else
		{
			_head = std::move(p);
		}
		_tail = new_tail;
	}
};



template<typename T>
class simple_queue_dummy
{
private:
	struct node
	{
		std::shared_ptr<T>		data;
		std::unique_ptr<node>	next;
	};

	std::unique_ptr<node>	_head;
	node*					_tail;

public:
	simple_queue_dummy() :
		_head(new node), _tail(_head.get()) {}		// init. with dummy(no data) node as head and tail

	simple_queue_dummy(const simple_queue_dummy& other) = delete;
	simple_queue_dummy& operator=(const simple_queue_dummy&) = delete;

	std::shared_ptr<T> try_pop()
	{
		if (_head.get() == _tail)		// by dummy node
		{
			return std::shared_ptr<T>();
		}

		std::shared_ptr<T> const ptr(_head->data);
		std::unique_ptr<node> old = std::move(_head);
		_head = std::move(old->next);
		return ptr;
	}

	void push(T value)
	{
		std::shared_ptr<T> ptr{ std::make_shared<T>(std::move(value)) };
		std::unique_ptr<node> n(new node);
		node* const new_tail = n.get();
		_tail->data = ptr;					// set data to current tail
		_tail->next = std::move(n);			// link new node

		_tail = new_tail;		// set new tail
	}
};


template<typename T>
class threadsafe_queue66
{
private:
	struct node
	{
		std::shared_ptr<T>		data;
		std::unique_ptr<node>	next;
	};

	std::mutex		_head_mutex;
	std::mutex		_tail_mutex;

	std::unique_ptr<node>	_head;
	node*					_tail;		// _head and node.next are unique_ptr, so use raw ptr that doesn't yield transfer of ownership

	node* get_tail()
	{
		std::lock_guard<std::mutex> lock(_tail_mutex);
		return _tail;
	}

	std::unique_ptr<node> pop_head()
	{
		std::lock_guard<std::mutex> lock(_head_mutex);

		if (_head.get() == get_tail())
			return nullptr;

		std::unique_ptr<node> old = std::move(_head);
		_head = std::move(old->next);

		return old;
	}

public:
	threadsafe_queue66() : _head(new node), _tail(_head.get()) {}
	threadsafe_queue66(const threadsafe_queue66&) = delete;

	threadsafe_queue66& operator=(const threadsafe_queue66&) = delete;

	std::shared_ptr<T> try_pop()
	{
		std::unique_ptr<node> old = pop_head();
		return old ? old->data : std::shared_ptr<T>();
	}

	void push(T value)
	{
		// make value available
		std::shared_ptr<T> ptr{ std::make_shared<T>(std::move(value)) };

		// new dummy node
		std::unique_ptr<node> n{ new node };
		node* const new_tail = n.get();
		std::lock_guard<std::mutex> lock(_tail_mutex);
		_tail->data = ptr;				// set data
		_tail->next = std::move(n);		// link new dummy tail
		_tail = new_tail;				// set new dummy tail
	}
};


template<typename T>
class threadsafe_queue
{
private:
	struct node
	{
		std::shared_ptr<T>		data;
		std::unique_ptr<node>	next;
	};

	std::mutex		_head_mutex;
	std::mutex		_tail_mutex;
	std::condition_variable	_cond;

	std::unique_ptr<node>	_head;
	node*					_tail;		// _head and node.next are unique_ptr, so use raw ptr that doesn't yield transfer of ownership

	node* get_tail()
	{
		std::lock_guard<std::mutex> lock(_tail_mutex);
		return _tail;
	}

	std::unique_ptr<node> pop_head()
	{
		std::unique_ptr<node> old = std::move(_head);
		_head = std::move(old->next);
		return old;
	}

	std::unique_lock<std::mutex> wait_for_data()
	{
		std::unique_lock<std::mutex> head_lock(_head_mutex);
		_cond.wait(head_lock, [&] {return _head.get() != get_tail(); });
		return std::move(head_lock);
	}

	std::unique_ptr<node> wait_pop_head()
	{
		std::unique_lock<std::mutex> head_lock(wait_for_data());
		return pop_head();
	}

	std::unique_ptr<node> wait_pop_head(T& value)
	{
		std::unique_lock<std::mutex> head_lock(wait_for_data());
		value = std::move(*_head->data);
		return pop_head();
	}

	std::unique_ptr<node> try_pop_head()
	{
		std::lock_guard<std::mutex> head_lock(_head_mutex);
		if (_head.get() == get_tail())
			return std::unique_ptr<node>();

		return pop_head();
	}

	std::unique_ptr<node> try_pop_head(T& value)
	{
		std::lock_guard<std::mutex> lock(_head_mutex);
		if (_head.get() == get_tail())
			return std::unique_ptr<node>();

		value = std::move(*_head->data);
		return pop_head();
	}

public:
	threadsafe_queue() : _head(new node), _tail(_head.get()) {}
	threadsafe_queue(const threadsafe_queue&) = delete;

	threadsafe_queue& operator=(const threadsafe_queue&) = delete;

	std::shared_ptr<T> wait_and_pop()
	{
		std::unique_ptr<node> const old = wait_pop_head();
		return old->data;
	}

	void wait_and_pop(T& value)
	{
		std::unique_ptr<node> const old = wait_pop_head(value);
	}

	std::shared_ptr<T> try_pop()
	{
		std::unique_ptr<node> old = try_pop_head();
		return old ? old->data : std::shared_ptr<T>();
	}

	bool try_pop(T& value)
	{
		std::unique_ptr<node> const old = try_pop_head(value);
		return old ? true : false;
	}

	void push(T value)
	{
		// make value available
		std::shared_ptr<T> ptr{ std::make_shared<T>(std::move(value)) };

		// new dummy node
		std::unique_ptr<node> n{ new node };
		node* const new_tail = n.get();
		{	// for lock_guard
			std::lock_guard<std::mutex> lock(_tail_mutex);
			_tail->data = ptr;				// set data
			_tail->next = std::move(n);		// link new dummy tail
			_tail = new_tail;				// set new dummy tail
		}
		_cond.notify_one();
	}

	void empty() const
	{
		std::lock_guard<std::mutex> lock(_head_mutex);
		return (_head.get() == get_tail());
	}
};