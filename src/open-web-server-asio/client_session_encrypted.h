#ifndef NO_ENCRYPTION

#ifndef CLIENT_SESSION_ENCRYPTED_H
#define CLIENT_SESSION_ENCRYPTED_H

#include "client_session_base.h"
#include <boost/asio/ssl.hpp>

class ClientSessionEncrypted : public ClientSessionBase
{
public:
    //constructor/destructor
    ClientSessionEncrypted(boost::asio::io_service& io_service, boost::asio::ssl::context& context);
    virtual ~ClientSessionEncrypted();

    //non-virtual methods
    boost::asio::ssl::stream<boost::asio::ip::tcp::socket>::lowest_layer_type &ssl_socket();

    //virtual methods
    virtual void start();
    virtual void close_socket();
    virtual void async_read_some(std::vector<char> &buffer);
    virtual void async_write(std::vector<boost::asio::const_buffer> &buffers);
    virtual void async_write(std::vector<char> &buffer);
private:
    //variables
    boost::asio::ssl::stream<boost::asio::ip::tcp::socket> ssl_socket_;

    //methods
    void handle_handshake(const boost::system::error_code &error);

};

#endif // CLIENT_SESSION_ENCRYPTED_H
#endif // #ifndef NO_ENCRYPTION
