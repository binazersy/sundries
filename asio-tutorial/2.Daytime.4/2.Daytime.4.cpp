/**=======================================================================

	2.Daytime.4.cpp
	
		@author: binazersy@gmail.com
=======================================================================**/


#include <iostream>
#include <boost/asio.hpp>
#include <boost/array.hpp>


using boost::asio::ip::udp;


int main(int argc, char* argv[])
{
	try
	{
		std::string host{ argc != 2 ? "localhost" : argv[1] };

		boost::asio::io_context io;
		udp::resolver resolver(io);
		udp::endpoint reciverEndpoint = *resolver.resolve(udp::v4(), host, "daytime").begin();

		udp::socket socket(io);
		socket.open(udp::v4());

		boost::array<char, 1> buf = { { 0 } };
		socket.send_to(boost::asio::buffer(buf), reciverEndpoint);

		boost::array<char, 128> recvBuf;
		udp::endpoint senderEndpoint;
		size_t len = socket.receive_from(boost::asio::buffer(recvBuf), senderEndpoint);

		std::cout.write(recvBuf.data(), len);
	}
	catch (std::exception & e)
	{
		std::cerr << e.what() << std::endl;
	}

	return 0;
}

