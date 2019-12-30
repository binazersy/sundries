/**=======================================================================

	2.Daytime.cpp
	
		@author: binazersy@gmail.com
=======================================================================**/


#include <iostream>
#include <boost/array.hpp>
#include <boost/asio.hpp>


using boost::asio::ip::tcp;


void Daytime1(const std::string& host)
{
    boost::asio::io_context io_context;

    tcp::resolver resolver(io_context);
    tcp::resolver::results_type endpoints = resolver.resolve(host, "daytime");

    tcp::socket socket(io_context);
    boost::asio::connect(socket, endpoints);

    for (;;)
    {
        boost::array<char, 128> buf;
        boost::system::error_code error;

        size_t len = socket.read_some(boost::asio::buffer(buf), error);

        if (boost::asio::error::eof == error)
            break;
        else if (error)
            throw boost::system::system_error(error);

        std::cout.write(buf.data(), len);
    }
}


int main(int argc, char* argv[])
{
    try
    {
        std::string host{argc != 2 ? "localhost" : argv[1]};

        Daytime1(host);
    }
    catch (std::exception & e)
    {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}