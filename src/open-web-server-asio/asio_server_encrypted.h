#ifndef ASIO_SERVER_ENCRYPTED_H
#define ASIO_SERVER_ENCRYPTED_H

#include <cstdlib>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include "client_session_encrypted.h"
#include "asio_server_base.h"

using namespace boost::asio;

class AsioServerEncrypted : public AsioServerBase
{
public:
    //constructor. Starts the server and begins accepting connections
    AsioServerEncrypted(boost::asio::io_service &io_service, short port);
    virtual ~AsioServerEncrypted();

    //async callback: called each time a new client connects
    void handle_accept(ClientSessionEncrypted* new_session, const boost::system::error_code &error);
private:
  boost::asio::ssl::context context_;
};

#endif // ASIO_SERVER_H
