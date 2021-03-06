/**=======================================================================

	6.lock_based.cpp

		@author: binazersy@gmail.com

=======================================================================**/


#include "threadsafe_stack.hpp"
#include "threadsafe_queue.hpp"
#include "threadsafe_lookup_table.hpp"
#include <thread>
#include <iostream>
#include <sstream>


void provide(threadsafe_queue<int>* q)
{
	for (int i = 1; i < 100000; ++i)
	{
		q->push(i);
	}
}


void consume(threadsafe_queue<int>* q)
{
	int i = 1;
	while (true)
	{
		auto v = q->wait_and_pop();
		if (0 == (i++ % 10))
			std::cout << std::endl;
		std::cout << *v << " ";
	}
}


void provide1(threadsafe_queue66<int>* q)
{
	for (int i = 1; i < 100000; ++i)
	{
		q->push(i);
	}
}

void consume1(threadsafe_queue66<int>* q)
{
	int i = 1;

	while (true)
	{
		auto v = q->try_pop();
		if (v)
		{
			if (0 == (i++ % 10))
				std::cout << std::endl;
			std::cout << *v << " ";
		}
	}
}


void write_table(threadsafe_lookup_table<int, std::string> *t)
{
//	std::string temp;
	std::stringstream ss;
	
	for (int i = 0; i < 10000; ++i)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(100));

		ss.clear();
		ss << "Number: " << i << std::endl;
//		temp = i;
		t->add_or_update_mapping(i, ss.str());
	}
}


void read_table(threadsafe_lookup_table<int, std::string> *t)
{
	while (true)
	{
		const std::string & s = t->value_for(std::rand() % 10000);
		if (!s.empty())
			std::cout << s.c_str() << std::endl;
		else
			std::cout << "empty" << std::endl;
	}
}


void sub_63()
{
	threadsafe_lookup_table<int, std::string> lookup_table;

	lookup_table.add_or_update_mapping(99999, "kato sayaka");
	lookup_table.add_or_update_mapping(88888, "endo mai");

	std::thread input_thread1{ write_table, &lookup_table};

	const unsigned num = 5;
	std::vector<std::thread> cons;

	for (unsigned i = 0; i < num; ++i)
	{
		std::thread w{ read_table, &lookup_table };
		cons.push_back(std::move(w));
	}

	input_thread1.join();

	for (unsigned i = 0; i < num; ++i)
	{
		cons[i].join();
	}
}

void sub_62()
{
	threadsafe_queue_old<int> old_queue;
	threadsafe_queue66<int> queue66;
	threadsafe_queue<int> queue;

	//std::thread input_thread{ provide, &queue };

	//const unsigned num = 5;
	//std::vector<std::thread> cons;
	
	//for (unsigned i =0; i < num; ++i)
	//{
	//	std::thread w{ consume, &queue };
	//	cons.push_back(std::move(w));
	//}

	//input_thread.join();

	//for (unsigned i = 0; i < num; ++i)
	//{
	//	cons[i].join();
	//}


	std::thread input_thread1{ provide1, &queue66 };

	const unsigned num = 5;
	std::vector<std::thread> cons;

	for (unsigned i = 0; i < num; ++i)
	{
		std::thread w{ consume1, &queue66 };
		cons.push_back(std::move(w));
	}

	input_thread1.join();

	for (unsigned i = 0; i < num; ++i)
	{
		cons[i].join();
	}
}


int main()
{
	threadsafe_stack<int> st;
	st.push(3);
	auto i = st.pop();

	sub_63();

	getchar();

	return 0;
}