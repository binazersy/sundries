/**=======================================================================

	10.state.cpp

		@author: binazersy@gmail.com

		State Pattern

=======================================================================**/


#include <iostream>
#include <memory>
#include <sstream>
#include "gumball_machine.h"
#include "state.h"


//!---------------------------------------------------------
//! @brief Main Entry Function
//!---------------------------------------------------------
//! 
int main()
{
	std::shared_ptr<GumballMachine> machine = std::make_shared<GumballMachine>();
	machine->initialize(2);

	std::cout << machine->toString();

	machine->insertQuarter();
	machine->turnCrank();

	std::cout << machine->toString();

	machine->insertQuarter();
	machine->turnCrank();
	machine->insertQuarter();
	machine->turnCrank();

	machine->refill(5);
	machine->insertQuarter();
	machine->turnCrank();

	std::cout << machine->toString();

	machine.reset();

	getchar();

    return 0;
}

