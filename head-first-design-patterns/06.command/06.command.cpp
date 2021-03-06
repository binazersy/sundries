/**=======================================================================

	06.command.cpp

		@author: binazersy@gmail.com

=======================================================================**/


#include <memory>
#include <vector>
#include <iostream>
#include <string>
#include <sstream>


//!---------------------------------------------------------
//! @brief Command Interface
//!---------------------------------------------------------
//! 
class Command
{
public:
	virtual void execute() = 0;
};


class NoCommand : public Command
{
public:
	void execute() {}
};


class Light
{
	std::string _location = "";

public:
	Light(std::string location)
	{
		_location = location;
	}

	void on()
	{
		std::cout << _location << " light is on" << std::endl;
	}
	void off() 
	{
		std::cout << _location << " light is off" << std::endl;
	}
};


class CeilingFan
{
	const enum LEVEL
	{
		OFF = 0,
		LOW,
		MEDIUM,
		HIGH
	};

	std::string _location = "";
	LEVEL _level;

public:
	CeilingFan(std::string location)
	{
		_location = location;
	}

	void high()
	{
		// turns the ceiling fan on to high
		_level = HIGH;
		std::cout << _location << " ceiling fan is on high" << std::endl;
	}

	void medium()
	{
		// turns the ceiling fan on to medium
		_level = MEDIUM;
		std::cout << _location << " ceiling fan is on medium" << std::endl;
	}

	void low()
	{
		// turns the ceiling fan on to low
		_level = LOW;
		std::cout << _location << " ceiling fan is on low" << std::endl;
	}

	void off()
	{
		// turns the ceiling fan off
		_level = OFF;
		std::cout << _location << " ceiling fan is off" << std::endl;
	}

	int getSpeed()
	{
		return _level;
	}
};


class GarageDoor 
{
	std::string _location;

public:
	GarageDoor(std::string location)
	{
		_location = location;
	}

	void up()
	{
		std::cout << _location << " garage Door is Up" << std::endl;
	}

	void down()
	{
		std::cout << _location + " garage Door is Down" << std::endl;
	}

	void stop()
	{
		std::cout << _location + " garage Door is Stopped" << std::endl;
	}

	void lightOn()
	{
		std::cout << _location + " garage light is on" << std::endl;
	}

	void lightOff()
	{
		std::cout << _location + " garage light is off" << std::endl;
	}
};


class Hottub 
{
	bool _on;
	int _temperature;

public:
	Hottub()
	{
	}

	void on()
	{
		_on = true;
	}

	void off()
	{
		_on = false;
	}

	void bubblesOn()
	{
		if (_on)
		{
			std::cout << "Hottub is bubbling!" << std::endl;
		}
	}

	void bubblesOff()
	{
		if (_on)
		{
			std::cout << "Hottub is not bubbling" << std::endl;
		}
	}

	void jetsOn()
	{
		if (_on)
		{
			std::cout << "Hottub jets are on" << std::endl;
		}
	}

	void jetsOff()
	{
		if (_on)
		{
			std::cout << "Hottub jets are off" << std::endl;
		}
	}

	void setTemperature(int temperature)
	{
		_temperature = temperature;
	}

	void heat()
	{
		_temperature = 105;
		std::cout << "Hottub is heating to a steaming 105 degrees" << std::endl;
	}

	void cool()
	{
		_temperature = 98;
		std::cout << "Hottub is cooling to 98 degrees" << std::endl;
	}
};


class Stereo
{
	std::string _location;

public:
	Stereo(std::string location)
	{
		_location = location;
	}

	void on()
	{
		std::cout << _location << " stereo is on" <<std::endl;
	}

	void off()
	{
		std::cout << _location << " stereo is off" << std::endl;
	}

	void setCD()
	{
		std::cout << _location << " stereo is set for CD input" << std::endl;
	}

	void setDVD()
	{
		std::cout << _location << " stereo is set for DVD input" << std::endl;
	}

	void setRadio()
	{
		std::cout << _location << " stereo is set for Radio" << std::endl;
	}

	void setVolume(int volume)
	{
		// code to set the volume
		// valid range: 1-11 (after all 11 is better than 10, right?)
		std::cout << _location << " Stereo volume set to " << volume << std::endl;
	}
};


class TV
{
	std::string _location;
	int _channel;

public:
	TV(std::string location)
	{
		_location = location;
	}

	void on()
	{
		std::cout << "TV is on" <<std::endl;
	}

	void off()
	{
		std::cout << "TV is off" << std::endl;
	}

	void setInputChannel()
	{
		_channel = 3;
		std::cout << "Channel is set for VCR" << std::endl;
	}
};


class LightOnCommand : public Command
{
private:
	std::shared_ptr<Light>	_light;

public:
	explicit LightOnCommand(std::shared_ptr<Light>& light) : _light(light) {}

	void execute() { if (_light) _light->on(); }
};


class LightOffCommand : public Command
{
private:
	std::shared_ptr<Light>	_light;

public:
	explicit LightOffCommand(std::shared_ptr<Light>& light) : _light(light) {}

	void execute() { if (_light) _light->off(); }
};


class CeilingFanOnCommand : public Command
{
	std::shared_ptr<CeilingFan> _fan;
	
public:
	CeilingFanOnCommand(std::shared_ptr<CeilingFan>& fan) :_fan(fan) {}

	void execute()
	{
		_fan->high();
	}
};


class CeilingFanOffCommand : public Command
{
	std::shared_ptr<CeilingFan> _fan;

public:
	CeilingFanOffCommand(std::shared_ptr<CeilingFan>& fan) :_fan(fan) {}

	void execute()
	{
		_fan->off();
	}
};


class GarageDoorUpCommand : public Command
{
	std::shared_ptr<GarageDoor> _door;

public:
	GarageDoorUpCommand(std::shared_ptr<GarageDoor>& door) :_door(door) {}

	void execute()
	{
		_door->up();
	}
};


class GarageDoorDownCommand : public Command
{
	std::shared_ptr<GarageDoor> _door;

public:
	GarageDoorDownCommand(std::shared_ptr<GarageDoor>& door) :_door(door) {}

	void execute()
	{
		_door->down();
	}
};


class HottubOnCommand : public Command
{
	std::shared_ptr<Hottub> _hottub;

public:
	HottubOnCommand(std::shared_ptr<Hottub>& hottub) :_hottub(hottub) {}

	void execute()
	{
		_hottub->on();
		_hottub->heat();
		_hottub->bubblesOn();
	}
};


class HottubOffCommand : public Command
{
	std::shared_ptr<Hottub> _hottub;

public:
	HottubOffCommand(std::shared_ptr<Hottub>& hottub) :_hottub(hottub) {}

	void execute()
	{
		_hottub->off();
	}
};


class SimpleRemocon
{
private:
	std::unique_ptr<Command> _slot;

public:
	SimpleRemocon() {}

	void setCommand(std::unique_ptr<Command>& command)
	{
		_slot = std::move(command);
	}

	void buttonWasPressed()
	{
		if (_slot)
			_slot->execute();
	}
};


//!---------------------------------------------------------
//! @brief Remote Control Class (Invoker)
//!---------------------------------------------------------
//! 
class RemoteControl
{
	std::vector<std::shared_ptr<Command>> _onCommands;
	std::vector<std::shared_ptr<Command>> _offCommands;

public:
	RemoteControl()
	{
		_onCommands.resize(7);
		_offCommands.resize(7);
	}

	void setCommand(unsigned slot, std::shared_ptr<Command>&& onCommand, std::shared_ptr<Command>&& offCommand)
	{
		if (slot >= _onCommands.size())
			return;

		_onCommands[slot] = onCommand;
		_offCommands[slot] = offCommand;
	}

	void onButtonWasPushed(unsigned slot)
	{
		if (slot >= _onCommands.size() || !_onCommands[slot])
			return;

		_onCommands[slot]->execute();
	}

	void offButtonWasPushed(unsigned slot)
	{
		if (slot >= _onCommands.size() || !_onCommands[slot])
			return;

		_offCommands[slot]->execute();
	}

	std::string toString()
	{
		std::stringstream ss;

		ss << "\n------ Remote Control -------\n";

		for (unsigned i = 0; i < _onCommands.size(); i++)
		{
			if (_onCommands[i])
			{
				ss << "[slot " << i << "] " << typeid(*_onCommands[i]).name()
					<< "    " << typeid(*_offCommands[i]).name() << std::endl;
			}
		}

		return ss.str();
	}
};


int main()
{
	std::unique_ptr<RemoteControl> remoteControl = std::make_unique<RemoteControl>();

	std::shared_ptr<Light> livingRoomLight = std::make_shared<Light>("Living Room");
	std::shared_ptr<Light> kitchenLight = std::make_shared<Light>("Kitchen");
	std::shared_ptr<CeilingFan> ceilingFan = std::make_shared<CeilingFan>("Living Room");
	std::shared_ptr<GarageDoor> garageDoor = std::make_shared<GarageDoor>("Main house");
	std::shared_ptr<Stereo> stereo = std::make_shared<Stereo>("Living Room");

	std::shared_ptr<LightOnCommand> livingRoomLightOn = std::make_shared<LightOnCommand>(livingRoomLight);
	std::shared_ptr<LightOffCommand> livingRoomLightOff = std::make_shared<LightOffCommand>(livingRoomLight);
	std::shared_ptr<LightOnCommand> kitchenLightOn = std::make_shared<LightOnCommand>(kitchenLight);
	std::shared_ptr<LightOffCommand> kitchenLightOff = std::make_shared<LightOffCommand>(kitchenLight);

	std::shared_ptr<CeilingFanOnCommand> ceilingFanOn = std::make_shared<CeilingFanOnCommand>(ceilingFan);
	std::shared_ptr<CeilingFanOffCommand> ceilingFanOff = std::make_shared<CeilingFanOffCommand>(ceilingFan);

	std::shared_ptr<GarageDoorUpCommand> garageDoorUp = std::make_shared<GarageDoorUpCommand>(garageDoor);
	std::shared_ptr<GarageDoorDownCommand> garageDoorDown = std::make_shared<GarageDoorDownCommand>(garageDoor);

	remoteControl->setCommand(0, livingRoomLightOn, livingRoomLightOff);
	remoteControl->setCommand(1, kitchenLightOn, kitchenLightOff);
	remoteControl->setCommand(2, ceilingFanOn, ceilingFanOff);
	remoteControl->setCommand(4, garageDoorUp, garageDoorDown);

	std::cout << remoteControl->toString() << std::endl;

	std::cout << "Controlling..." << std::endl;

	remoteControl->onButtonWasPushed(0);
	remoteControl->offButtonWasPushed(0);
	remoteControl->onButtonWasPushed(1);
	remoteControl->offButtonWasPushed(1);
	remoteControl->onButtonWasPushed(2);
	remoteControl->offButtonWasPushed(2);
	remoteControl->onButtonWasPushed(3);
	remoteControl->offButtonWasPushed(3);
	remoteControl->onButtonWasPushed(4);
	remoteControl->offButtonWasPushed(4);
	remoteControl->onButtonWasPushed(5);

	getchar();

    return 0;
}

