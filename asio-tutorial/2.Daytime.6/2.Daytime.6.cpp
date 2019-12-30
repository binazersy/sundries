/**=======================================================================

	2.Daytime.6.cpp
	
		@author: binazersy@gmail.com
=======================================================================**/


#include <ctime>
#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/array.hpp>
#include <boost/shared_ptr.hpp>


using boost::asio::ip::udp;


std::string makeDaytimeString()
{
	using namespace std;
	char buf[128];
	time_t now = time(0);
	ctime_s(buf, sizeof(buf), &now);

	return std::string(buf);
}



class UdpServer
{
public:
	UdpServer(boost::asio::io_context& io)
		: _socket(io, udp::endpoint(udp::v4(), 13))
	{
		startReceive();
	}

private:
	void startReceive()
	{
		_socket.async_receive_from(boost::asio::buffer(_recv), _remoteEndpoint,
			boost::bind(&UdpServer::handleReceive, this,
				boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred));
	}

	void handleReceive(const boost::system::error_code& error, std::size_t /*bt*/)
	{
		if (!error)
		{
			boost::shared_ptr<std::string> message(new std::string(makeDaytimeString()));

			_socket.async_send_to(boost::asio::buffer(*message), _remoteEndpoint,
				boost::bind(&UdpServer::handleSend, this, message,
					boost::asio::placeholders::error,
					boost::asio::placeholders::bytes_transferred));

			startReceive();
		}
	}

	void handleSend(boost::shared_ptr<std::string> /*message*/, const boost::system::error_code& /*error*/, std::size_t bytesTransferred)
	{
		std::cout << bytesTransferred << " bytes transferred." << std::endl;
	}

private:
	udp::socket _socket;
	udp::endpoint _remoteEndpoint;
	boost::array<char, 1> _recv;
};


int main()
{
	try
	{
		boost::asio::io_context io;
		UdpServer server(io);
		io.run();
	}
	catch (std::exception & e)
	{
		std::cerr << e.what() << std::endl;
	}

	return 0;
}

