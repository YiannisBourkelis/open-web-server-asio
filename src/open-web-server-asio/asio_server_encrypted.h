#ifndef NO_ENCRYPTION

#ifndef ASIO_SERVER_ENCRYPTED_H
#define ASIO_SERVER_ENCRYPTED_H

#include <cstdlib>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include "client_session_encrypted.h"
#include "asio_server_base.h"
#include <boost/asio/ssl.hpp>

using namespace boost::asio;

class AsioServerEncrypted : public AsioServerBase
{
public:
    //constructor. Starts the server and begins accepting connections
    AsioServerEncrypted(boost::asio::io_service &io_service, short port, std::string certificate_chain_file, std::string private_key_file);
    virtual ~AsioServerEncrypted();

    //async callback: called each time a new client connects
    void handle_accept(ClientSessionEncrypted* new_session, const boost::system::error_code &error);
private:
  boost::asio::ssl::context context_;
  std::string _certificate_chain_file;
  std::string _private_key_file;
};

#endif // ASIO_SERVER_H

#endif // #ifndef NO_ENCRYPTION
