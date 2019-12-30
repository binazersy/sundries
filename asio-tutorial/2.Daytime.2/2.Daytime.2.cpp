/**=======================================================================

	2.Daytime.2.cpp
    
		@author: binazersy@gmail.com
=======================================================================**/


#include <iostream>
#include <boost/asio.hpp>
#include <chrono>
#include <iomanip>


#pragma warning(disable: 4996)
using boost::asio::ip::tcp;


std::string makeDaytimeString()
{
    //time_t now = time(0);
    //return ctime(&now);
    //std::chrono::system_clock

    using namespace std::chrono;
    auto t = system_clock::to_time_t(system_clock::now());
    
    std::ostringstream oss;
    oss << std::put_time(std::localtime(&t), "%Y-%m-%d %H-%M-%S");

    return oss.str();
}


int main()
{
    try
    {
        boost::asio::io_context io;
        tcp::acceptor acceptor(io, tcp::endpoint(tcp::v4(), 13));

        for (;;)
        {
            tcp::socket socket(io);
            acceptor.accept(socket);

            std::string dt = makeDaytimeString();

            boost::system::error_code ignoredError;
            boost::asio::write(socket, boost::asio::buffer(dt), ignoredError);
        }
    }
    catch (std::exception & e)
    {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}