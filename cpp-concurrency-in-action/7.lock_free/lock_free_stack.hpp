/**=======================================================================

	lock_free_stack.hpp

		@author: binazersy@gmail.com

=======================================================================**/


#pragma once

#ifndef _ENABLE_ATOMIC_ALIGNMENT_FIX
#define _ENABLE_ATOMIC_ALIGNMENT_FIX
#endif

#include <atomic>
#include <memory>


template<typename T>
class lock_free_stack
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

	void try_reclaim(node* old_head)
	{
		// only thread?
		if (1 == _threads_in_pop)		
		{
			// get pending nodes
			node* nodes_to_delete = _to_be_deleted.exchange(nullptr);

			// still only thread?
			if (0 == --_threads_in_pop)
			{
				delete_nodes(nodes_to_delete);
			}
			else if (nodes_to_delete)
			{
				// rechain
				chain_pending_nodes(nodes_to_delete);
			}

			// anyway, current node is free to delete
			delete old_head;
		}
		else
		{
			// chain current node
			chain_pending_nodes(old_head, old_head);
			--_threads_in_pop;
		}
	}

	void chain_pending_nodes(node* nodes)
	{
		node* last = nodes;
		while (node* const next = last->next)
		{
			last = next;
		}
		chain_pending_nodes(nodes, last);
	}

	void chain_pending_nodes(node* first, node* last)
	{
		last->next = _to_be_deleted;
		while (!_to_be_deleted.compare_exchange_weak(last->next, first));
	}

public:
	lock_free_stack() : _threads_in_pop(0) {}
	lock_free_stack(const lock_free_stack& other) = delete;
	lock_free_stack& operator=(const lock_free_stack& other) = delete;

	void push(T const& data)
	{
		node* const new_node = new node(data);
		new_node->next = _head.load();
		while (!_head.compare_exchange_weak(new_node->next, new_node));
	}

	std::shared_ptr<T> pop()
	{
		++_threads_in_pop;

		node* old_head = _head.load();
		while (old_head && !_head.compare_exchange_weak(old_head, old_head->next));

		std::shared_ptr<T> res;
		if (old_head)
		{
			res.swap(old_head->data);
		}
		try_reclaim(old_head);
		
		return res;
	}
};