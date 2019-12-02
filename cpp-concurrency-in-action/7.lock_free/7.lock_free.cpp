/**=======================================================================

	7.lock_free.cpp

		@author: binazersy@gmail.com

=======================================================================**/


#include "lock_free_stack.hpp"
#include "lock_free_stack_with_hp.hpp"
#include "lock_free_stack_with_rc.hpp"
#include "lock_free_queue.hpp"


int main()
{
	lock_free_stack<int> st1;
	lock_free_stack_with_hp<int> st2;
	lock_free_stack_with_rc<int> st3;

	lock_free_queue<int> q1;

	return 0;
}