#ifndef ASIO_SERVER_PLAIN_H
#define ASIO_SERVER_PLAIN_H

#include <cstdlib>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include "client_session_plain.h"
#include "asio_server_base.h"

using namespace boost::asio;

class AsioServerPlain : public AsioServerBase
{
public:
    //constructor. Starts the server and begins accepting connections
    AsioServerPlain(boost::asio::io_service &io_service, short port);
    virtual ~AsioServerPlain();

    //async callback: called each time a new client connects
    void handle_accept(ClientSessionPlain* new_session, const boost::system::error_code &error);
};

#endif // ASIO_SERVER_H
