/**=======================================================================

	4.x3.parallel_quick_sort.cpp
	
		@author: binazersy@gmail.com

=======================================================================**/


#include <list>
#include <future>
#include <iostream>
#include <functional>		// std::result_of


//!---------------------------------------------------------
//! @brief Sequential Quick Sort(Naive)
//!---------------------------------------------------------
//! 
template<typename T>
std::list<T> sequential_quick_sort(std::list<T> input)
{
	if (input.empty())
		return input;

	std::list<T> result;
	result.splice(result.begin(), input, input.begin());		// ㅁ : pivot
	T const& pivot = *result.begin();

	// rearrange
	auto divide_point = std::partition(input.begin(), input.end(), [&](T const& t) { return t < pivot; });

	// divide
	std::list<T> lower_part;
	lower_part.splice(lower_part.end(), input, input.begin(), divide_point);

	auto new_lower(sequential_quick_sort(std::move(lower_part)));
	auto new_higher(sequential_quick_sort(std::move(input)));

	result.splice(result.end(), new_higher);	// ㅁ + higher
	result.splice(result.begin(), new_lower);	// lower + ㅁ + higher

	return result;
}


//!---------------------------------------------------------
//! @brief Parallel Quick Sort(Naive)
//!---------------------------------------------------------
//! 
template<typename T>
std::list<T> parallel_quick_sort(std::list<T> input)
{
	if (input.empty())
		return input;

	std::list<T> result;
	result.splice(result.begin(), input, input.begin());		// ㅁ : pivot
	T const& pivot = *result.begin();

	// rearrange
	auto divide_point = std::partition(input.begin(), input.end(), [&](T const& t) { return t < pivot; });

	// divide
	std::list<T> lower_part;
	lower_part.splice(lower_part.end(), input, input.begin(), divide_point);

	// lower with another thread
	std::future<std::list<T> > new_lower(std::async(&parallel_quick_sort<T>, std::move(lower_part)));

	// upper with current thread
	auto new_higher(parallel_quick_sort(std::move(input)));
	//std::future<std::list<T> > new_higher(std::async(&parallel_quick_sort<T>, std::move(input)));

	result.splice(result.end(), new_higher);	// ㅁ + higher
	result.splice(result.begin(), new_lower.get());	// lower + ㅁ + higher

	return result;
}


template <typename S> void prt(const S& s)
{
	std::cout << s.size() << " elements: ";

	for (const auto& p : s)
	{
		std::cout << "(" << p << ") ";
	}

	std::cout << std::endl;
}


//template<typename F, typename A>
//std::future<typename std::result_of<F(A&&)>::type>
//spawn_task(F&& f, A&& a)
//{
//	typename std::result_of<F(A&&)>::type result_type;
//	std::packaged_task<result_type(A&&)>
//		task(std::move(f)));
//		std::future<result_type> res(task.get_future());
//		std::thread t(std::move(task), std::move(a));
//		t.detach();
//		return res;
//}

template<typename F, typename A>
std::future<typename std::result_of<F(A&&)>::type>
spawn_task(F&& f, A&& a)
{
	typedef std::result_of<F(A&&)>::type result_type;
	std::packaged_task<result_type(A&&)>
		task(std::move(f));
	std::future<result_type> res(task.get_future());
	std::thread t(std::move(task), std::move(a));
	t.detach();
	return res;
}



int main()
{
	std::list<int> values = { 5,7,3,4,1,9,2,8,10,6 };

	prt(values);

	auto sorted = sequential_quick_sort(values);

	std::cout << "After seq. sort:" << std::endl;
	prt(sorted);

	auto psorted = parallel_quick_sort(values);

	std::cout << "After par. sort:" << std::endl;
	prt(psorted);

	getchar();
    return 0;
}


