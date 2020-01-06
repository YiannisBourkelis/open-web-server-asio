#include "client_session_plain.h"

ClientSessionPlain::ClientSessionPlain(boost::asio::io_service& io_service) :
     ClientSessionBase(io_service), socket_(io_service)
{
}

ClientSessionPlain::~ClientSessionPlain()
{
}

//returns the active client session socket
ip::tcp::socket& ClientSessionPlain::socket()
{
    return socket_;
}

void ClientSessionPlain::close_socket()
{
    socket_.close();
}

void ClientSessionPlain::start()
{
    //Read some data from the client and invoke the callback
    //to proccess the client request
    async_read_some(client_request_.buffer);
}

void ClientSessionPlain::async_read_some(std::vector<char> &buffer)
{
    socket_.async_read_some(boost::asio::buffer(buffer.data(), buffer.size()),
                            boost::bind(&ClientSessionPlain::handle_read, this,
                            boost::asio::placeholders::error,
                            boost::asio::placeholders::bytes_transferred));
}

void ClientSessionPlain::async_read_some(std::vector<char> &buffer, size_t begin_offset, size_t max_size)
{
    socket_.async_read_some(boost::asio::buffer(buffer.data() + begin_offset, max_size),
                            boost::bind(&ClientSessionPlain::handle_read, this,
                            boost::asio::placeholders::error,
                            boost::asio::placeholders::bytes_transferred));
}

void ClientSessionPlain::async_write(std::vector<boost::asio::const_buffer> &buffers)
{
    boost::asio::async_write(socket_,
                             buffers,
                             boost::bind(&ClientSessionPlain::handle_write, this,
                             boost::asio::placeholders::error));
}

void ClientSessionPlain::async_write(std::vector<char> &buffer)
{
    boost::asio::async_write(socket_,
                             boost::asio::buffer(buffer.data(), buffer.size()),
                             boost::bind(&ClientSessionPlain::handle_write, this,
                             boost::asio::placeholders::error));
}
