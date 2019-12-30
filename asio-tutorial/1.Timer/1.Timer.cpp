/**=======================================================================

	1.Timer.cpp

		@author: binazersy@gmail.com
=======================================================================**/


#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/thread.hpp>


void Timer1()
{
	std::cout << "Timer1..." << std::endl;

	boost::asio::io_context io;
	boost::asio::steady_timer t(io, boost::asio::chrono::seconds(5));

	t.wait();

	std::cout << "\tHello world!" << std::endl;
}


void print2(const boost::system::error_code& /*e*/)
{
	std::cout << "\tHello, world!" << std::endl;
}


void Timer2()
{
	std::cout << "Timer2..." << std::endl;

	boost::asio::io_context io;
	boost::asio::steady_timer t(io, boost::asio::chrono::seconds(5));

	t.async_wait(&print2);

	io.run();
}


void print3(const boost::system::error_code& /*e*/,
	boost::asio::steady_timer* t, int* count)
{
	if (*count < 5)
	{
		std::cout << "\t" << *count << std::endl;
		++(*count);
		t->expires_at(t->expiry() + boost::asio::chrono::seconds(1));
		t->async_wait(boost::bind(print3, boost::asio::placeholders::error, t, count));
	}
}


void Timer3()
{
	std::cout << "Timer3..." << std::endl;

	boost::asio::io_context io;
	int count = 0;

	boost::asio::steady_timer t(io, boost::asio::chrono::seconds(1));
	t.async_wait(boost::bind(print3, boost::asio::placeholders::error, &t, &count));
	io.run();

	std::cout << "\tFinal count is " << count << std::endl;
}


class Printer
{
public:
	Printer(boost::asio::io_context& io)
		: _timer(io, boost::asio::chrono::seconds(1)), _count(0)
	{
		_timer.async_wait(boost::bind(&Printer::print, this));
	}

	~Printer()
	{
		std::cout << "\tFinal count is " << _count << std::endl;
	}

	void print()
	{
		if (_count < 5)
		{
			std::cout << "\t" << _count << std::endl;
			++_count;

			_timer.expires_at(_timer.expiry() + boost::asio::chrono::seconds(1));
			_timer.async_wait(boost::bind(&Printer::print, this));
		}
	}

private:
	boost::asio::steady_timer _timer;
	int _count;
};


void Timer4()
{
	std::cout << "Timer4..." << std::endl;
	boost::asio::io_context io;
	Printer printer(io);
	io.run();
}


class Printer5
{
public:
	Printer5(boost::asio::io_context& io)
		: _strand(io),
		_timer1(io, boost::asio::chrono::seconds(1)),
		_timer2(io, boost::asio::chrono::seconds(1)),
		_count(0)
	{
		_timer1.async_wait(boost::bind(&Printer5::print1, this));
		_timer2.async_wait(boost::bind(&Printer5::print2, this));
	}

	~Printer5()
	{
		std::cout << "\tFinal count is " << _count << std::endl;
	}

	void print1()
	{
		if (_count < 10)
		{
			std::cout << "\tTimer 1: " << _count << std::endl;
			++_count;

			_timer1.expires_at(_timer1.expiry() + boost::asio::chrono::seconds(1));
			_timer1.async_wait(boost::bind(&Printer5::print1, this));
		}
	}

	void print2()
	{
		if (_count < 10)
		{
			std::cout << "\tTimer 2: " << _count << std::endl;
			++_count;

			_timer2.expires_at(_timer2.expiry() + boost::asio::chrono::seconds(1));
			_timer2.async_wait(boost::bind(&Printer5::print2, this));
		}
	}

private:
	boost::asio::io_context::strand _strand;
	boost::asio::steady_timer _timer1;
	boost::asio::steady_timer _timer2;
	int _count;
};


void Timer5()
{
	std::cout << "Timer5..." << std::endl;

	boost::asio::io_context io;
	Printer5 pr(io);
	boost::thread t(boost::bind(&boost::asio::io_context::run, &io));
	io.run();
	t.join();
}


int main()
{
	Timer1();
	Timer2();
	Timer3();
	Timer4();
	Timer5();

	return 0;
}
