/**=======================================================================

	lock_free_stack_with_rc.h
	
		@author: binazersy@gmail.com
		
=======================================================================**/


#pragma once

#ifndef _ENABLE_ATOMIC_ALIGNMENT_FIX
#define _ENABLE_ATOMIC_ALIGNMENT_FIX
#endif

#include <atomic>
#include <memory>


template<typename T>
class lock_free_stack_with_rc
{
private:
	struct node;

	struct counted_node_ptr
	{
		int		external_count;
		node*	ptr;
	};

	struct node
	{
		std::shared_ptr<T>	data;
		std::atomic<int>	internal_count;
		counted_node_ptr	next;

		node(T const& data_)
			: data(std::make_shared<T>(data_)), internal_count(0) {}
	};

	std::atomic<counted_node_ptr>	_head;

	void increase_head_count(counted_node_ptr& old_counter)
	{
		counted_node_ptr new_counter;
		do
		{
			new_counter = old_counter;
			++new_counter.external_count;
		} while (!_head.compare_exchange_strong(old_counter, new_counter));
		// fine grained
		//	std::memory_order_acquire, std::memory_order_relaxed));

		old_counter.external_count = new_counter.external_count;
	}

public:
	lock_free_stack_with_rc() {}
	lock_free_stack_with_rc(const lock_free_stack_with_rc& other) = delete;
	lock_free_stack_with_rc& operator=(const lock_free_stack_with_rc& other) = delete;

	~lock_free_stack_with_rc()
	{
		while (pop());
	}

	void push(T const& data)
	{
		counted_node_ptr new_node;
		new_node.ptr = new node(data);
		new_node.external_count = 1;
		new_node.ptr->next = _head.load(std::memory_order_relaxed);
		while (!_head.compare_exchange_weak(new_node.ptr->next, new_node));
		// fine grained , std::memory_order_release, std::memory_order_relaxed));
	}

	std::shared_ptr<T> pop()
	{
		counted_node_ptr old_head = _head.load();	// fine grained , std::memory_order_relaxed);
		while (true)
		{
			increase_head_count(old_head);
			node* const ptr = old_head.ptr;
			if (!ptr)
			{
				return std::shared_ptr<T>();
			}

			if (_head.compare_exchange_strong(old_head, ptr->next))
				// fine grained , std::memory_order_relaxed))
			{
				std::shared_ptr<T> res;
				res.swap(ptr->data);

				const int count_increase = old_head.external_count - 2;
				if (ptr->internal_count.fetch_add(count_increase) == -count_increase)
				// fine grained
				//if (ptr->internal_count.fetch_add(count_increase, std::memory_order_release)
				//	== -count_increase)
				{
					delete ptr;
				}

				return res;
			}
			else if (ptr->internal_count.fetch_sub(1) == 1)
			{
				delete ptr;
			}
			// fine grained
			//else if (ptr->internal_count.fetch_add(-1, std::memory_order_relaxed) == 1)
			//{
			//	ptr->internal_count.load(std::memory_order_acquire);
			//	delete ptr;
			//}
		}
	}
};