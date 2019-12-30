/**=======================================================================

	2.Daytime.5.cpp
	
		@author: binazersy@gmail.com
=======================================================================**/


#include <ctime>
#include <iostream>
#include <string>
#include <boost/array.hpp>
#include <boost/asio.hpp>


using boost::asio::ip::udp;


std::string makeDaytimeString()
{
	using namespace std;
	char buf[128];
	time_t now = time(0);
	ctime_s(buf, sizeof(buf), &now);

	return std::string(buf);
}


int main()
{
	try
	{
		boost::asio::io_context io;

		udp::socket socket(io, udp::endpoint(udp::v4(), 13));

		for (;;)
		{
			boost::array<char, 1> recv;
			udp::endpoint remoteEndpoint;
			boost::system::error_code error;

			socket.receive_from(boost::asio::buffer(recv), remoteEndpoint);

			std::string message = makeDaytimeString();

			boost::system::error_code ignoredError;
			socket.send_to(boost::asio::buffer(message), remoteEndpoint, 0, ignoredError);
		}
	}
	catch (std::exception & e)
	{
		std::cerr << e.what() << std::endl;
	}

	return 0;
}

