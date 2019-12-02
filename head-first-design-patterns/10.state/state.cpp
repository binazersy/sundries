/**=======================================================================

	state.cpp
	
		@author: binazersy@gmail.com

=======================================================================**/


#include "state.h"
#include "gumball_machine.h"


NoQuarterState::NoQuarterState(std::shared_ptr<GumballMachine>&& machine)
{
	_machine = machine;
	//_machine = std::make_shared<GumballMachine>(*machine);
}

void NoQuarterState::insertQuarter()
{
	std::cout << "You inserted a quarter" << std::endl;
	_machine->setState(_machine->getHasQuarterState());
}


SoldState::SoldState(std::shared_ptr<GumballMachine>&& machine)
{
	_machine = machine;
	//_machine = std::make_shared<GumballMachine>(*machine);
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


SoldOutState::SoldOutState(std::shared_ptr<GumballMachine>&& machine)
{
	_machine = machine;
	//_machine = std::make_shared<GumballMachine>(*machine);
}

void SoldOutState::refill()
{
	_machine->setState(_machine->getNoQuarterState());
}


HasQuarterState::HasQuarterState(std::shared_ptr<GumballMachine>&& machine)
{
	_machine = machine;
	//_machine = std::make_shared<GumballMachine>(*machine);
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

