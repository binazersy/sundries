/**=======================================================================

	2.Daytime.3.cpp
	
		@author: binazersy@gmail.com
=======================================================================**/


#include <ctime>
#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/enable_shared_from_this.hpp>


using boost::asio::ip::tcp;


std::string makeDaytimeString()
{
	using namespace std;
	char buf[128];
	time_t now = time(0);
	ctime_s(buf, sizeof(buf), &now);

	return std::string(buf);
}


class TcpConnection : public boost::enable_shared_from_this<TcpConnection>
{
public:
	typedef boost::shared_ptr<TcpConnection> pointer;

	static pointer create(boost::asio::io_context& io)
	{
		return pointer(new TcpConnection(io));
	}

	tcp::socket& socket()
	{
		return _socket;
	}

	void start()
	{
		_message = makeDaytimeString();

		boost::asio::async_write(_socket, boost::asio::buffer(_message),
			boost::bind(&TcpConnection::handleWrite, shared_from_this(),
				boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred));
	}

private:
	TcpConnection(boost::asio::io_context& io) : _socket(io) {}

	void handleWrite(const boost::system::error_code& error, size_t bytesTransferred)
	{
		std::cout << bytesTransferred << " bytes transferred." << std::endl;
	}

	tcp::socket _socket;
	std::string _message;
};


class TcpServer
{
public:
	TcpServer(boost::asio::io_context& io)
		: _acceptor(io, tcp::endpoint(tcp::v4(), 13))
	{
		startAccept();
	}

private:
	void startAccept()
	{
		TcpConnection::pointer conn = TcpConnection::create(_acceptor.get_executor().context());

		_acceptor.async_accept(conn->socket(),
			boost::bind(&TcpServer::handleAccept, this, conn, boost::asio::placeholders::error));
	}

	void handleAccept(TcpConnection::pointer conn, const boost::system::error_code& error)
	{
		if (!error)
		{
			conn->start();
		}

		startAccept();
	}

private:
	tcp::acceptor _acceptor;
};

int main()
{
	try
	{
		boost::asio::io_context io;
		TcpServer server(io);
		io.run();
	}
	catch (std::exception & e)
	{
		std::cerr << e.what() << std::endl;
	}

	return 0;
}

