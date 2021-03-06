/**=======================================================================

	01.strategy.cpp

		@author: binazersy@gmail.com

=======================================================================**/


#include "Duck.h"


int main()
{
	std::cout << "=== Mallard Duck" << std::endl;
	std::unique_ptr<Duck> mallard = std::make_unique<MallardDuck>();
	mallard->performQuack();
	mallard->performFly();

	std::cout << "=== Model Duck" << std::endl;
	std::unique_ptr<Duck> model = std::make_unique<ModelDuck>();
	model->performFly();
	model->setFlyBehavior(std::make_unique<FlyRocketPowered>());
	model->performFly();
	model->setQuackBehavior(std::make_unique<Squeak>());
	model->performQuack();

	std::cout << "=== Another Mallard Duck" << std::endl;
	auto mallard2 = makeMallardDuck();
	mallard2->performFly();
	mallard2->performQuack();
	mallard2.reset();

	std::cout << "=== Another Model Duck" << std::endl;
	auto model2 = makeModelDuck();
	model2->performFly();
	model2->performQuack();
	model2.reset();

	std::getchar();

    return 0;
}

