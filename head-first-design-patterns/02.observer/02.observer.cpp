/**=======================================================================

	02.observer.cpp

		@author: binazersy@gmail.com

		!!!
		Subject의 observers는 shared_ptr가 합당한듯 하다.
		근데 Observer 생성자에서 registerObserver할 shared_ptr를 얻을 수 없다.
		(enable_shared_from_this를 사용한다 하더라도)
		그래서 트릭으로 생성자에서 custom deleter(delete를 하지 않는)를 갖는 shared_ptr를 만든다

		굳이 Observer에서 Subject를 유지할 필요가 없다면(no polling)
		Observer 생성자에서 register하지 말고 밖으로 빼는게 나을 듯 하다.
		(이 경우 Subject는 상황에 따라 unique_ptr로)
		
		EMC++.20에서 weak_ptr가 observer 패턴에 적합하다 했다.
		맞는 말이지 싶어 해보려니
		removeObserver()에서 원 shared_ptr(observer)로 부터 파생된 weak_ptr가 뭔지 알수가 없다.
		아니 알려고 하는게 이상하다.

=======================================================================**/


#include <list>
#include <memory>
#include <algorithm>
#include <iostream>


//!---------------------------------------------------------
//! @brief Observer Interface
//!---------------------------------------------------------
//! 
class Observer
{
public:
	virtual void update(float temp, float humidity, float pressure) = 0;
};


//!---------------------------------------------------------
//! @brief Subject Interface
//!---------------------------------------------------------
//! 
class Subject
{
public:
	virtual void registerObserver(std::shared_ptr<Observer>&& o) = 0;
	virtual void removeObserver(std::shared_ptr<Observer>&& o) = 0;
	virtual void notifyObservers() = 0;
};


class DisplayElement
{
public:
	virtual void display() = 0;
};


//!---------------------------------------------------------
//! @brief Weather Data class
//!---------------------------------------------------------
//! 
class WeatherData : public Subject
{
private:
	std::list<std::shared_ptr<Observer> >	_observers;

	float _temperature;
	float _humidity;
	float _pressure;

public:
	WeatherData() {}


	//! @brief Register observer to list
	//! @param o	shared_ptr to Observer object
	//!---------------------------------------------------------
	//!
	void registerObserver(std::shared_ptr<Observer>&& o)
	{
		_observers.emplace_back(o);
	}

	//! @brief Remove observer from list
	//! @param o	shared_ptr to Observer object 
	//!---------------------------------------------------------
	//!
	void removeObserver(std::shared_ptr<Observer>&& o)
	{
		_observers.remove(o);
	}

	//! @brief Notify to obeservers
	//!---------------------------------------------------------
	//!
	void notifyObservers()
	{
		std::for_each(_observers.begin(), _observers.end(),
			[this](const std::shared_ptr<Observer>& o)
			{
				if (o) o->update(_temperature, _humidity, _pressure);
			}
		);
	}

	void measurementsChanged()
	{
		notifyObservers();
	}

	void setMeasurements(float temp, float humi, float press)
	{
		_temperature = temp;
		_humidity = humi;
		_pressure = press;

		measurementsChanged();
	}
};


//!---------------------------------------------------------
//! @brief Current Condition Display 
//!---------------------------------------------------------
//! 
class CurrentConditionDisplay : public Observer, public DisplayElement, public std::enable_shared_from_this<CurrentConditionDisplay>
{
private:
	float _temperature;
	float _humidity;

	std::shared_ptr<Subject>	_weatherData;

public:
	CurrentConditionDisplay(std::shared_ptr<Subject>&& weatherData)
	{
		_weatherData = weatherData;
		
		auto ptr = std::shared_ptr<CurrentConditionDisplay>(this, [](CurrentConditionDisplay *) {});
		_weatherData->registerObserver(shared_from_this());
	}

	void update(float temp, float humidity, float pressure)
	{
		_temperature = temp;
		_humidity = humidity;
		display();
	}

	void display()
	{
		std::cout << "Current conditions: " << _temperature << "F degrees, " << _humidity << " humidity" << std::endl;
	}
};


//!---------------------------------------------------------
//! @brief Statistics Display Class
//!---------------------------------------------------------
//! 
class StatisticsDisplay : public Observer, public DisplayElement, public std::enable_shared_from_this<StatisticsDisplay>
{
private:
	float _maxTemp = 0.0f;
	float _minTemp = 200;
	float _tempSum = 0.0f;
	int _numReadings;
	std::shared_ptr<Subject> _weatherData;

public:
	StatisticsDisplay(std::shared_ptr<Subject>&& weatherData)
	{
		_weatherData = weatherData;
		
		auto ptr = std::shared_ptr<StatisticsDisplay>(this, [](StatisticsDisplay *) {});
		_weatherData->registerObserver(shared_from_this());
	}

	void update(float temp, float humidity, float pressure)
	{
		_tempSum += temp;
		_numReadings++;

		if (temp > _maxTemp)
			_maxTemp = temp;

		if (temp < _minTemp) 
			_minTemp = temp;

		display();
	}

	void display()
	{
		std::cout << "Avg/Max/Min temperature = " << (_tempSum / _numReadings) << "/" << _maxTemp << "/" << _minTemp << std::endl;
	}
};


//!---------------------------------------------------------
//! @brief Forecast Display Class
//!---------------------------------------------------------
//! 
class ForecastDisplay : public Observer, public DisplayElement, public std::enable_shared_from_this<ForecastDisplay>
{
private:
	float _currentPressure = 29.92f;
	float _lastPressure;
	std::shared_ptr<Subject> _weatherData;

public:
	ForecastDisplay(std::shared_ptr<Subject>&& weatherData)
	{
		_weatherData = weatherData;
		
		auto ptr = std::shared_ptr<ForecastDisplay>(this, [](ForecastDisplay *) {});
		_weatherData->registerObserver(shared_from_this());
	}

	void update(float temp, float humidity, float pressure)
	{
		_lastPressure = _currentPressure;
		_currentPressure = pressure;

		display();
	}

	void display()
	{
		std::cout << "Forecast: ";
		if (_currentPressure > _lastPressure)
		{
			std::cout << "Improving weather on the way!";
		}
		else if (_currentPressure == _lastPressure)
		{
			std::cout << "More of the same";
		}
		else if (_currentPressure < _lastPressure)
		{
			std::cout << "Watch out for cooler, rainy weather";
		}
		std::cout << std::endl;
	}
};


//!---------------------------------------------------------
//! @brief main entry function
//!---------------------------------------------------------
//! 
int main()
{
	// subject
	std::shared_ptr<WeatherData> data = std::make_shared<WeatherData>();

	// observers
	std::shared_ptr<CurrentConditionDisplay> disp = std::make_shared<CurrentConditionDisplay>(data);
	std::shared_ptr<StatisticsDisplay> sd = std::make_shared<StatisticsDisplay>(data);
	std::shared_ptr<ForecastDisplay> fd = std::make_shared<ForecastDisplay>(data);
	//CurrentConditionDisplay* disp = new CurrentConditionDisplay(data);
	//StatisticsDisplay* sd = new StatisticsDisplay(data);
	//ForecastDisplay* fd = new ForecastDisplay(data);

	data->setMeasurements(80, 65, 30.4f);
	data->setMeasurements(82, 70, 29.2f);
	data->setMeasurements(78, 90, 29.2f);

	//delete disp;
	sd.reset();
	fd.reset();

	getchar();

    return 0;
}

