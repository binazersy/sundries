/**=======================================================================

	state.cpp
	
		@author: binazersy@gmail.com

=======================================================================**/


#include "state.h"
#include "gumball_machine.h"


NoQuarterState::NoQuarterState(GumballMachine* machine)
	: State(machine)
{
}


void NoQuarterState::insertQuarter()
{
	std::cout << "You inserted a quarter" << std::endl;
	_machine->setState(_machine->getHasQuarterState());
}


SoldState::SoldState(GumballMachine* machine)
	: State(machine)
{
}


void SoldState::dispense()
{
	_machine->releaseBall();
	if (_machine->getCount() > 0)
	{
		_machine->setState(_machine->getNoQuarterState());
	}
	else
	{
		std::cout << "Oops, out of gumballs!" << std::endl;
		_machine->setState(_machine->getSoldOutState());
	}
}


SoldOutState::SoldOutState(GumballMachine* machine)
	: State(machine)
{
}


void SoldOutState::refill()
{
	_machine->setState(_machine->getNoQuarterState());
}


HasQuarterState::HasQuarterState(GumballMachine* machine)
	: State(machine)
{
}


void HasQuarterState::ejectQuarter()
{
	std::cout << "Quarter returned" << std::endl;
	_machine->setState(_machine->getNoQuarterState());
}


void HasQuarterState::turnCrank()
{
	std::cout << "You turned..." << std::endl;
	_machine->setState(_machine->getSoldState());
}

