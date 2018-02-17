#ifndef CLIENT_SESSION_H
#define CLIENT_SESSION_H

#include <cstdlib>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <QByteArray>
#include <QString>
#include "client_request.h"
#include "client_request_parser.h"

using namespace boost::asio;

class ClientSession
{
public:
    ClientSession (boost::asio::io_service& io_service);

    //methods
    ip::tcp::socket &socket();
    void start();

private:
    //variables
    ip::tcp::socket socket_;
    const int REQUEST_BUFFER_SIZE = 1024;

    ClientRequest client_request_;
    ClientRequestParser client_request_parser_;

    //methods
    void handle_read(const boost::system::error_code &error, size_t bytes_transferred);
    void handle_write(const boost::system::error_code &error);
};

#endif // CLIENT_SESSION_H
