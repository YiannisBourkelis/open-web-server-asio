#ifndef ASIO_SERVER_H
#define ASIO_SERVER_H

#include <cstdlib>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include "client_session.h"
//#include "server_config.h"

using namespace boost::asio;

class AsioServer
{
public:
    //constructor. Starts the server and begins accepting connections
    AsioServer(boost::asio::io_service &io_service, short port);

    //async callback: called each time a new client connects
    void handle_accept(ClientSession* new_session, const boost::system::error_code &error);


private:
  boost::asio::io_service& io_service_;
  ip::tcp::acceptor acceptor_;
};

#endif // ASIO_SERVER_H
