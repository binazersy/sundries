/**=======================================================================

	8.5.parallel_accumulate.cpp

		@author: binazersy@gmail.com

		!!! 8.4�� ����.

=======================================================================**/


#include <iostream>
#include <thread>
#include <vector>
#include <algorithm>
#include <numeric>
#include <future>
#include <stdexcpt.h>


//!---------------------------------------------------------
//! @brief Parallel version of accmulate using std::async
//!---------------------------------------------------------
//! 
template<typename Iterator, typename T>
T parallel_accumulate(Iterator first, Iterator last, T init)
{
	unsigned long const length = std::distance(first, last);
	unsigned long const max_chunk_size = 25;
	if (length <= max_chunk_size)
	{
		return std::accumulate(first, last, init);
	}
	else
	{
		Iterator mid_point = first;
		std::advance(mid_point, length / 2);
		
		std::future<T> first_half_result =
			std::async(parallel_accumulate<Iterator, T>, first, mid_point, init);

		// !!!!! max_chunk_size �� ���� ���� �ſ� ���� ���¿��� ������ �ݵ� ���ķ� ������ �����(����)�� �ɸ���.
		// async�� �ѱ��� �ʴ� �� �κ��� ���� ������ �����ϳ� parallel�� �ƴ����̴�.
		// ������ ���� standard�� �����ų� max_chunk_size�� �÷��ָ� ������ �̰� �׳� ����̴�.
		//T second_half_result = parallel_accumulate(mid_point, last, T());
		T second_half_result = std::accumulate(mid_point, last, T());
		return first_half_result.get() + second_half_result;
	}
}


int main()
{
	std::vector<long> values(100000);

	int i = 0;
	for (auto& x : values)
	{
		x = ++i;
	}

	try
	{

		int total_v = std::accumulate(values.begin(), values.end(), 0);

		int total = parallel_accumulate(values.begin(), values.end(), 0);

		std::cout << "Total: " << total << " (" << total_v << ")" << std::endl;
	}
	catch (std::exception &e)
	{
		std::cout << e.what() << std::endl;
	}

	getchar();

	return 0;
}

