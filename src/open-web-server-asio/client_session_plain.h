#ifndef CLIENT_SESSION_PLAIN_H
#define CLIENT_SESSION_PLAIN_H

#include "client_session_base.h"

class ClientSessionPlain : public ClientSessionBase
{
public:
    //constructor/destructor
    ClientSessionPlain(boost::asio::io_service& io_service);
    virtual ~ClientSessionPlain();

    //non-virtual methods
    ip::tcp::socket &socket();

    //virtual methods
    virtual void start();
    virtual void close_socket();
    virtual void async_read_some(std::vector<char> &buffer);
    virtual void async_write(std::vector<boost::asio::const_buffer> &buffers);
    virtual void async_write(std::vector<char> &buffer);

private:
    //variables
    ip::tcp::socket socket_;
};

#endif // CLIENT_SESSION_PLAIN_H
