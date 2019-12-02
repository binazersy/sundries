/**=======================================================================

	gumball_machine.cpp
	
		@author: binazersy@gmail.com

=======================================================================**/


#include "gumball_machine.h"
#include "state.h"


void GumballMachine::initialize(unsigned balls)
{
	_count = balls;

	_soldOutState.reset();
	_noQuarterState.reset();
	_hasQuarterState.reset();
	_soldState.reset();

	_soldOutState = std::make_shared<SoldOutState>(shared_from_this());
	_noQuarterState = std::make_shared<NoQuarterState>(shared_from_this());
	_hasQuarterState = std::make_shared<HasQuarterState>(shared_from_this());
	_soldState = std::make_shared<SoldState>(shared_from_this());

	_state.reset();
	if (balls > 0)
		_state = _noQuarterState;
}


void GumballMachine::insertQuarter()
{
	_state->insertQuarter();
}

void GumballMachine::ejectQuarter()
{
	_state->ejectQuarter();
}


void GumballMachine::turnCrank()
{
	_state->turnCrank();
	_state->dispense();
}

void GumballMachine::setState(std::shared_ptr<State>& state)
{
	_state = state;
}

std::shared_ptr<State>& GumballMachine::getState()
{
	return _state;
}

std::shared_ptr<State>& GumballMachine::getHasQuarterState()
{
	return _hasQuarterState;
}

std::shared_ptr<State>& GumballMachine::getSoldOutState()
{
	return _soldOutState;
}

std::shared_ptr<State>& GumballMachine::getNoQuarterState()
{
	return _noQuarterState;
}

std::shared_ptr<State>& GumballMachine::getSoldState()
{
	return _soldState;
}

void GumballMachine::releaseBall()
{
	std::cout << "A gumball comes rolling out the slot..." << std::endl;

	if (0 != _count) --_count;
}

void GumballMachine::refill(unsigned count)
{
	_count += count;
	std::cout << "The gumball machine was just refilled; it's new count is: " << _count << std::endl;
	_state->refill();
}


std::string GumballMachine::toString()
{
	std::stringstream ss;

	ss << "\nMighty Gumball, Inc.";
	ss << "\nC++-enabled Standing Gumball Model #2004";
	ss << "\nInventory: " << _count << " gumball";
	if (_count != 1)
	{
		ss << "s";
	}
	ss << "\n";
	ss << "Machine is " << _state->toString() << "\n";

	return ss.str();
}
