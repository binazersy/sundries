/**=======================================================================

	4.x2a.splice_partition.cpp

		@author: binazersy@gmail.com

=======================================================================**/


#include <list>
#include <iostream>
#include <algorithm>


std::list<int> l = { 5,7,3,4,1,9,2,8,10,6 };
std::list<int> r;


void prt()
{
	std::cout << "L: ";
	std::for_each(l.begin(), l.end(), [](int& i) { std::cout << i << " "; });
	std::cout << std::endl;

	std::cout << "R: ";
	std::for_each(r.begin(), r.end(), [](int& i) { std::cout << i << " "; });
	std::cout << std::endl;
}


int main()
{
	std::cout << "Initial" << std::endl;
	prt();

	std::cout << "Pivot(splice 1st)" << std::endl;
	r.splice(r.begin(), l, l.begin());
	prt();

	std::cout << "Partition" << std::endl;
	const int& pivot = *r.begin();
	auto divide = std::partition(l.begin(), l.end(), [&](const int& i) { return i < pivot; });
	prt();

	std::cout << "Divide(splice by pivot)" << std::endl;
	std::list<int> lower;
	lower.splice(lower.end(), l, l.begin(), divide);
	prt();

	std::cout << "Lower: ";
	std::for_each(lower.begin(), lower.end(), [](int& i) { std::cout << i << " "; });
	std::cout << std::endl;


	getchar();
	return 0;
}