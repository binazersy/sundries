/**=======================================================================

	1.1.simple_hello.cpp
	
		@author: binazersy@gmail.com

=======================================================================**/


#include <iostream>
#include <thread>


void hello()
{
	std::cout << "Hello Concurrent World\n";
}


int main()
{
	std::thread t(hello);
	t.join();

    return 0;
}

