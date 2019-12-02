/**=======================================================================

	Duck.h

		@author: binazersy@gmail.com

=======================================================================**/


#include <memory>
#include <iostream>


//!---------------------------------------------------------
//! @brief Fly Behavior Interface
//!---------------------------------------------------------
//! 
class FlyBehavior
{
public:
	virtual void fly() = 0;
};


//!---------------------------------------------------------
//! @brief Quack Behavior Interface
//!---------------------------------------------------------
//! 
class QuackBehavior
{
public:
	virtual void quack() = 0;
};


//!---------------------------------------------------------
//! @brief Duck
//!---------------------------------------------------------
//! 
class Duck
{
protected:
	std::unique_ptr<FlyBehavior> _flyBehavior;
	std::unique_ptr<QuackBehavior> _quackBehavior;

public:
	Duck() {}
	virtual void display() {}

	void performFly()
	{
		_flyBehavior->fly();
	}

	void performQuack()
	{
		_quackBehavior->quack();
	}

	void swim()
	{
		std::cout << "All ducks float, even decoys!" << std::endl;
	}

	//! @brief Sets a fly behavior
	//! @param fb	unique_ptr to FlyBehavior object.
	//!---------------------------------------------------------
	//!
	void setFlyBehavior(std::unique_ptr<FlyBehavior>&& fb)
	{
		_flyBehavior = std::move(fb);
	}

	//! @brief Sets a quack behavior
	//! @param qb	unique_ptr to QuackBehavior object.
	//!---------------------------------------------------------
	//!
	void setQuackBehavior(std::unique_ptr<QuackBehavior>&& qb)
	{
		_quackBehavior = std::move(qb);
	}
};


class FlyWithWings : public FlyBehavior
{
public:
	void fly()
	{
		std::cout << "I'm flying!!" << std::endl;
	}
};


class FlyNoWay : public FlyBehavior
{
public:
	void fly()
	{
		std::cout << "I can't fly" << std::endl;
	}
};


class FlyRocketPowered : public FlyBehavior
{
public:
	void fly()
	{
		std::cout << "I¡¯m flying with a rocket!" << std::endl;
	}
};


class Quack : public QuackBehavior
{
public:
	void quack()
	{
		std::cout << "Quack" << std::endl;
	}
};


class MuteQuack : public QuackBehavior
{
public:
	void quack()
	{
		std::cout << "<< Silence >>" << std::endl;
	}
};


class Squeak : public QuackBehavior
{
public:
	void quack()
	{
		std::cout << "Squeak" << std::endl;
	}
};


class MallardDuck : public Duck
{
public:
	MallardDuck()
	{
		_flyBehavior = std::make_unique<FlyWithWings>();
		_quackBehavior = std::make_unique<Quack>();
	}

	void display()
	{
		std::cout << "I¡¯m a real mallard duck" << std::endl;
	}
};


class ModelDuck : public Duck
{
public:
	ModelDuck()
	{
		_flyBehavior = std::make_unique<FlyNoWay>(FlyNoWay());
		_quackBehavior = std::make_unique<Quack>(Quack());
	}

	void display()
	{
		std::cout << "I¡¯m a model duck" << std::endl;
	}
};


//! @brief Custom deleter for Duck(Example)
//! @param duck	Ptr. to Duck
//!---------------------------------------------------------
//!
auto delDuck = [](Duck* duck)
{
	//! @todo 2019-09-05 correct english!!!
	std::cout << "Mallard Duck is being delete." << std::endl;
	delete duck;
};


//! @brief Make Mallard Duck (Example)
//! @param params	Not used.
//! @return unique_ptr of Duck
//!---------------------------------------------------------
//!
template<typename... Ts>
std::unique_ptr<Duck, decltype(delDuck)> makeMallardDuck(Ts... params)
{
	std::unique_ptr<Duck, decltype(delDuck)> p(nullptr, delDuck);
	p.reset(new MallardDuck);

	// you may pass parameters by
	// p.reset(new SomeDuck(std::forward<Ts>(params)...));

	return p;
}


//! @brief Make Model Duck (Example)
//! @details return type deduction(C++14) and local deleter.
//!			 you may merge this with above into one.
//!			 but, this is just example. use std::make_unique instead.
//! @param params	Not used.
//! @return unique_ptr of Duck
//!---------------------------------------------------------
//!
template<typename... Ts>
auto makeModelDuck(Ts... params)
{
	auto delModel = [](Duck *duck)
	{
		std::cout << "Model Duck is ..." << std::endl;
		delete duck;
	};

	std::unique_ptr<Duck, decltype(delModel)> p(nullptr, delModel);
	p.reset(new ModelDuck());

	return p;
}