#ifndef ASIO_SERVER_BASE_H
#define ASIO_SERVER_BASE_H

#include <cstdlib>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>

using namespace boost::asio;

class AsioServerBase
{
public:
    //constructor. Starts the server and begins accepting connections
    AsioServerBase(boost::asio::io_service &io_service, short port);
    virtual ~AsioServerBase();

protected:
  boost::asio::io_service& io_service_;
  ip::tcp::acceptor acceptor_;
};

#endif // ASIO_SERVER_H
