/**=======================================================================

	state.h

		@author: binazersy@gmail.com

=======================================================================**/


#pragma once


#include <iostream>
#include <memory>


class GumballMachine;


//!---------------------------------------------------------
//! @brief State Class (Base)
//!---------------------------------------------------------
//! 
class State
{
protected:
	std::shared_ptr<GumballMachine> _machine;
	//GumballMachine * _machine;

public:
	State() : _machine(nullptr) {}
	virtual void insertQuarter() = 0;
	virtual void ejectQuarter() = 0;
	virtual void turnCrank() = 0;
	virtual void dispense() = 0;
	virtual void refill() = 0;
	virtual std::string toString() = 0;
};


class NoQuarterState : public State
{

public:
	//NoQuarterState(GumballMachine* machine);
	NoQuarterState(std::shared_ptr<GumballMachine>&& machine);
	~NoQuarterState() { std::cout << "~NoQuarterState()" << std::endl; }


	void insertQuarter();
	void ejectQuarter()
	{
		std::cout << "You haven¡¯t inserted a quarter" << std::endl;
	}

	void turnCrank()
	{
		std::cout << "You turned, but there¡¯s no quarter" << std::endl;
	}

	void dispense()
	{
		std::cout << "You need to pay first" << std::endl;
	}

	void refill() {}

	std::string toString()
	{
		return "waiting for quarter";
	}
};


class SoldState : public State
{
public:
	//SoldState(GumballMachine* machine);
	SoldState(std::shared_ptr<GumballMachine>&& machine);

	void insertQuarter()
	{
		std::cout << "Please wait, we're already giving you a gumball" << std::endl;
	}

	void ejectQuarter()
	{
		std::cout << "Sorry, you already turned the crank" << std::endl;
	}

	void turnCrank()
	{
		std::cout << "Turning twice doesn't get you another gumball!" << std::endl;
	}

	void dispense();

	void refill() { }

	std::string toString() { return "dispensing a gumball"; }
};


class SoldOutState : public State
{
public:
	//SoldOutState(GumballMachine* machine);
	SoldOutState(std::shared_ptr<GumballMachine>&& machine);

	void insertQuarter()
	{
		std::cout << "You can't insert a quarter, the machine is sold out" << std::endl;
	}

	void ejectQuarter()
	{
		std::cout << "You can't eject, you haven't inserted a quarter yet" << std::endl;
	}

	void turnCrank()
	{
		std::cout << "You turned, but there are no gumballs" << std::endl;
	}

	void dispense()
	{
		std::cout << "No gumball dispensed" << std::endl;
	}

	void refill();

	std::string toString()
	{
		std::string str("sold out");
		return str;
	}
};


class HasQuarterState : public State
{
public:
	//HasQuarterState(GumballMachine* machine);
	HasQuarterState(std::shared_ptr<GumballMachine>&& machine);

	void insertQuarter()
	{
		std::cout << "You can¡¯t insert another quarter" << std::endl;
	}

	void ejectQuarter();
	void turnCrank();

	void dispense()
	{
		std::cout << "No gumball dispensed" << std::endl;
	}

	void refill() {}

	std::string toString()
	{
		return "waiting for turn of crank";
	}
};

