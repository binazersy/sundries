/**=======================================================================

	gumball_machine.h

		@author: binazersy@gmail.com

=======================================================================**/


#pragma once


#include <iostream>
#include <memory>
#include <sstream>


class State;


class GumballMachine : public std::enable_shared_from_this<GumballMachine>
{
	std::shared_ptr<State> _soldOutState;
	std::shared_ptr<State> _noQuarterState;
	std::shared_ptr<State> _hasQuarterState;
	std::shared_ptr<State> _soldState;

	std::shared_ptr<State> _state;
	unsigned _count = 0;

public:
	void initialize(unsigned balls);
	
	void insertQuarter();
	void ejectQuarter();
	void turnCrank();
	void setState(std::shared_ptr<State>& state);
	std::shared_ptr<State>& getState();
	std::shared_ptr<State>& getHasQuarterState();
	std::shared_ptr<State>& getSoldOutState();
	std::shared_ptr<State>& getNoQuarterState();
	std::shared_ptr<State>& getSoldState();

	void releaseBall();
	void refill(unsigned count);
	unsigned getCount() const { return _count; }

	std::string toString();
};
