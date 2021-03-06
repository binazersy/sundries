/**=======================================================================

	03.decorator.cpp

		@author: binazersy@gmail.com

=======================================================================**/


#include <string>
#include <iostream>


//!---------------------------------------------------------
//! @brief Beverage Class
//!---------------------------------------------------------
//! 
class Beverage
{
protected:
	std::string _desc;

public:
	virtual std::string getDescription() { return _desc; }
	virtual double cost() = 0;
};


//!---------------------------------------------------------
//! @brief Condiment Decorator Class
//!---------------------------------------------------------
//! 
class CondimentDecorator : public Beverage
{
protected:
	std::unique_ptr<Beverage> _beverage;

public:
	CondimentDecorator(std::unique_ptr<Beverage>& beverage)
		: _beverage(std::move(beverage))
	{}

	virtual std::string getDescription() = 0;
};


class Espresso : public Beverage
{
public:
	Espresso() { _desc = "Espresso"; }
	double cost() { return 1.99; }
};


class HouseBlend : public Beverage
{
public:
	HouseBlend() { _desc = "House Blend Coffee"; }
	double cost() { return 0.89;  }
};


class DarkRoast : public Beverage
{
public:
	DarkRoast() { _desc = "Dark Roast Coffee"; }
	double cost() { return .99; }
};


class Decaf : public Beverage
{
public:
	Decaf() { _desc = "Decaf Coffee"; }
	double cost() { return 1.05; }
};


class Mocha : public CondimentDecorator
{
public:
	Mocha(std::unique_ptr<Beverage>& beverage)
		: CondimentDecorator(beverage)
	{
	}

	std::string getDescription()
	{
		return _beverage ? _beverage->getDescription() + ", Mocha" : "Mocha";
	}

	double cost()
	{
		return _beverage ? (.20 + _beverage->cost()) : .20;
	}
};


class Soy : public CondimentDecorator
{
public:
	Soy(std::unique_ptr<Beverage>& beverage)
		: CondimentDecorator(beverage)
	{
	}

	std::string getDescription() { return _beverage ? _beverage->getDescription() + ", Soy" : "Soy"; }
	double cost() { return _beverage ? (.15 + _beverage->cost()) : .15; }
};


class Milk : public CondimentDecorator
{
public:
	Milk(std::unique_ptr<Beverage>& beverage)
		: CondimentDecorator(beverage)
	{
	}

	std::string getDescription() { return _beverage ? _beverage->getDescription() + ", Milk" : "Milk"; }
	double cost() { return _beverage ? (.10 + _beverage->cost()) : .10; }
};


class Whip : public CondimentDecorator
{
public:
	Whip(std::unique_ptr<Beverage>& beverage)
		: CondimentDecorator(beverage) 
	{
	}

	std::string getDescription() { return _beverage ? _beverage->getDescription() + ", Whip" : "Whip"; }
	double cost() { return _beverage ? (.10 + _beverage->cost()) : .10; }
};


//!---------------------------------------------------------
//! @brief Main Entry Function
//!---------------------------------------------------------
//! 
int main()
{
	std::unique_ptr<Beverage> beverage = std::make_unique<Espresso>();
	std::cout << beverage->getDescription() << " $" << beverage->cost() << std::endl;

	std::unique_ptr<Beverage> beverage2 = std::make_unique<DarkRoast>();
	beverage2 = std::make_unique<Mocha>(beverage2);
	beverage2 = std::make_unique<Mocha>(beverage2);
	beverage2 = std::make_unique<Whip>(beverage2);
	std::cout << beverage2->getDescription() << " $" << beverage2->cost() << std::endl;

	std::unique_ptr<Beverage> beverage3 = std::make_unique<HouseBlend>();
	beverage3 = std::make_unique<Soy>(beverage3);
	beverage3 = std::make_unique<Mocha>(beverage3);
	beverage3 = std::make_unique<Whip>(beverage3);
	std::cout << beverage3->getDescription() << " $" << beverage3->cost() << std::endl;

	// if you want this, write default constructor
	//std::unique_ptr<Beverage> beverage4 = std::make_unique<Whip>();
	//std::cout << beverage4->getDescription() << " $" << beverage4->cost() << std::endl;

	getchar();
}

