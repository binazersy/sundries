/**=======================================================================

	gumball_machine.h

		@author: binazersy@gmail.com

=======================================================================**/


#pragma once


#include <iostream>
#include <memory>
#include <sstream>


class State;


class GumballMachine
{
	std::unique_ptr<State> _soldOutState;
	std::unique_ptr<State> _noQuarterState;
	std::unique_ptr<State> _hasQuarterState;
	std::unique_ptr<State> _soldState;

	State*	_state = nullptr;
	unsigned _count = 0;

public:
	void initialize(unsigned balls);
	
	void insertQuarter();
	void ejectQuarter();
	void turnCrank();
	void setState(std::unique_ptr<State>& state);
	std::unique_ptr<State>& getHasQuarterState();
	std::unique_ptr<State>& getSoldOutState();
	std::unique_ptr<State>& getNoQuarterState();
	std::unique_ptr<State>& getSoldState();

	void releaseBall();
	void refill(unsigned count);
	unsigned getCount() const { return _count; }

	std::string toString();
};
