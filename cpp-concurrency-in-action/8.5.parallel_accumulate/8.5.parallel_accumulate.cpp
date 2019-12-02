/**=======================================================================

	8.5.parallel_accumulate.cpp

		@author: binazersy@gmail.com

		!!! 8.4가 낫다.

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

		// !!!!! max_chunk_size 가 위와 같이 매우 작은 상태에서 나머지 반도 병렬로 돌리면 데드락(추정)에 걸린다.
		// async로 넘기지 않는 뒷 부분을 먼저 돌리면 동작하나 parallel이 아닐터이다.
		// 나머지 반을 standard로 돌리거나 max_chunk_size를 늘려주면 되지만 이건 그냥 찍기이다.
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

