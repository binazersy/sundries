/**=======================================================================

	08.template_method.cpp
	
		@author: binazersy@gmail.com

=======================================================================**/


#include <iostream>
#include <memory>


//!---------------------------------------------------------
//! @brief Beverage Class (Base)
//!---------------------------------------------------------
//! 
class Beverage
{
public:
	virtual void prepareReceipe() final
	{
		boilWater();
		brew();
		pourInCup();
		if (wantsCondiments())
			addCondiments();
	}

protected:
	virtual void boilWater()
	{
		std::cout << "Boiling water" << std::endl;
	}

	virtual void brew() = 0;

	void pourInCup()
	{
		std::cout << "Pouring into cup" << std::endl;
	}

	virtual void addCondiments() = 0;

	virtual bool wantsCondiments() { return true; }
};


class Coffee : public Beverage
{
protected:
	void brew() override
	{
		std::cout << "Dripping through filter" << std::endl;
	}
	
	void addCondiments() override
	{
		std::cout << "Adding sugar and milk" << std::endl;
	}
};


class Tea : public Beverage
{
protected:
	void brew() override
	{
		std::cout << "Steeping the tea" << std::endl;
	}

	void addCondiments() override
	{
		std::cout << "Adding lemon" << std::endl;
	}
};


class CoffeeWithHook : public Coffee
{
public:
	bool wantsCondiments() override
	{
		char answer = getUserInput();
		if ('y' == answer || 'Y' == answer)
			return true;
		else
			return false;
	}

private:
	char getUserInput()
	{
		char answer;
		std::cout << "Would you like sugar and milk with your coffee (y/n)? ";
		
		std::cin >> answer;
		return answer;
	}
};


int main()
{
	std::unique_ptr<Tea> tea = std::make_unique<Tea>();

	tea->prepareReceipe();

	std::unique_ptr<Coffee> coffee = std::make_unique<CoffeeWithHook>();

	coffee->prepareReceipe();

	getchar();

    return 0;
}

