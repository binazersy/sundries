/**=======================================================================

	lock_free_stack_with_hp.hpp

		@author: binazersy@gmail.com

=======================================================================**/


#pragma once

#include <atomic>
#include <memory>
#include <thread>
#include <stdexcept>


unsigned const max_hazard_pointers = 100;

struct hazard_pointer
{
	std::atomic<std::thread::id>	id;
	std::atomic<void*>				pointer;
};

hazard_pointer hazard_pointers[max_hazard_pointers];


class hp_owner
{
	hazard_pointer*	_hp;

public:
	hp_owner(const hp_owner&) = delete;
	hp_owner& operator=(const hp_owner&) = delete;

	hp_owner()
		: _hp(nullptr)
	{
		for (unsigned i = 0; i < max_hazard_pointers; ++i)
		{
			std::thread::id old_id;
			if (hazard_pointers[i].id.compare_exchange_strong(old_id, std::this_thread::get_id()))
			{
				_hp = &hazard_pointers[i];
				break;
			}
		}

		if (!_hp)
		{
			throw std::runtime_error("No hazard pointer available!!!");
		}
	}
	~hp_owner()
	{
		_hp->pointer.store(nullptr);
		_hp->id.store(std::thread::id());
	}

	std::atomic<void*>& get_pointer() { return _hp->pointer; }
};


std::atomic<void*>& get_hazard_pointer_for_current_thread()
{
	thread_local static hp_owner hazard;
	return hazard.get_pointer();
}


bool outstanding_hazard_pointers_for(void* p)
{
	for (unsigned i = 0; i < max_hazard_pointers; ++i)
	{
		if (hazard_pointers[i].pointer.load() == p)
			return true;
	}
	return false;
}


template<typename T>
void do_delete(void* p)
{
	delete static_cast<T*>(p);
}

struct data_to_reclaim
{
	void* data;
	std::function<void(void*)> deleter;
	data_to_reclaim* next;

	template<typename T> data_to_reclaim(T* p)
		: data(p), deleter(&do_delete<T>), next(nullptr)
	{
	}
	~data_to_reclaim()
	{
		deleter(data);
	}
};


std::atomic<data_to_reclaim*>	nodes_to_reclaim;

void add_to_reclaim_list(data_to_reclaim* node)
{
	node->next = nodes_to_reclaim.load();
	while (!nodes_to_reclaim.compare_exchange_strong(node->next, node));
}


template<typename T> void reclaim_later(T* data)
{
	add_to_reclaim_list(new data_to_reclaim(data));
}


void delete_nodes_with_no_hazards()
{
	data_to_reclaim* current = nodes_to_reclaim.exchange(nullptr);
	while (current)
	{
		data_to_reclaim* next = current->next;
		if (!outstanding_hazard_pointers_for(current->data))
		{
			delete current;
		}
		else
		{
			add_to_reclaim_list(current);
		}
		current = next;
	}
}


template<typename T>
class lock_free_stack_with_hp
{
private:
	struct node
	{
		std::shared_ptr<T>	data;
		node*				next;

		node(T const& data_) : data(std::make_shared<T>(data_)) {}
	};

	std::atomic<node*>		_head;
	std::atomic<unsigned>	_threads_in_pop;
	std::atomic<node*>		_to_be_deleted;

	static void delete_nodes(node* nodes)
	{
		while (nodes)
		{
			node* next = nodes->next;
			delete nodes;
			nodes = next;
		}
	}

public:
	lock_free_stack_with_hp() {}
	lock_free_stack_with_hp(const lock_free_stack_with_hp& other) = delete;
	lock_free_stack_with_hp& operator=(const lock_free_stack_with_hp& other) = delete;

	void push(T const& data)
	{
		node* const new_node = new node(data);
		new_node->next = _head.load();
		while (!_head.compare_exchange_weak(new_node->next, new_node));
	}

	std::shared_ptr<T> pop()
	{
		std::atomic<void*>& hp = get_hazard_pointer_for_current_thread();
		node* old_head = _head.load();
		do
		{
			node* temp;
			do
			{
				// setting hazard pointer
				temp = old_head;
				hp.store(old_head);
				old_head = _head.load();
			} while (old_head != temp);
		} while (old_head && !_head.compare_exchange_strong(old_head, old_head->next));

		hp.store(nullptr);

		std::shared_ptr<T> res;
		if (old_head)
		{
			res.swap(old_head->data);
			if (outstanding_hazard_pointers_for(old_head))
			{
				reclaim_later(old_head);
			}
			else
			{
				delete old_head;
			}

			delete_nodes_with_no_hazards();
		}

		return res;
	}
};