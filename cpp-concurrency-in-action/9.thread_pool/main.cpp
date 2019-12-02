/**=======================================================================

	main.cpp

		@author: binazersy@gmail.com

=======================================================================**/


#include <iostream>
#include "thread_pool.hpp"


int func()
{
	std::cout << "func" << std::endl;
	return 10000;
}

int main()
{
	thread_pool_simple pool;
	pool.submit([]() {std::cout << "hahaha" << std::endl; });


	thread_pool pool2;
	auto res = pool2.submit(func);

	std::cout << "Result: " << res.get() << std::endl;

	std::vector<long> values;

	for (long i = 1; i < 101; ++i)
	{
		values.push_back(i);
	}


	//auto v = parallel_accmulate_using_tp(values.begin(), values.end(), (long)0);
	//std::cout << "Acc: " << v << std::endl;


	std::list<long> to_sort = { 7,3,5,1, 9, 11, 4, 2, 6, 8, 13,10 };

	std::cout << "Before:" << std::endl;
	std::for_each(to_sort.begin(), to_sort.end(), [&](long v) {std::cout << "(" << v << ") "; });
	std::cout << std::endl;

	auto sorted = parallel_quick_sort_using_tp(to_sort);

	std::cout << "Sorted:" << std::endl;
	std::for_each(sorted.begin(), sorted.end(), [&](long v) {std::cout << "(" << v << ") "; });
	std::cout << std::endl;


	getchar();

	return 0;
}

