#include "client_session.h"

//constructor
ClientSession::ClientSession(boost::asio::io_service& io_service) : socket_(io_service)
{
    //resize the buffer to accept the request.
    //data_.resize(REQUEST_BUFFER_SIZE);
}

//returns the active client session socket
ip::tcp::socket& ClientSession::socket()
{
    return socket_;
}

void ClientSession::start()
{
    //Read some data from the client and invoke the callback
    //to proccess the client request
    socket_.async_read_some(boost::asio::buffer(client_request_parser_.data_.data(), REQUEST_BUFFER_SIZE),
                            boost::bind(&ClientSession::handle_read, this,
                            boost::asio::placeholders::error,
                            boost::asio::placeholders::bytes_transferred));
}

void ClientSession::handle_read(const boost::system::error_code& error, size_t bytes_transferred)
{
    if (!error)
    {
        std::string respstr ("HTTP/1.1 200 OK\r\n"
                                   "Content-Type: text/html; charset=utf-8\r\n"
                                   "Content-Length: 5"
                                   "\r\n\r\n"
                                   "Hello");

        if (client_request_parser_.proccess_new_data(bytes_transferred, client_request_)){

            boost::asio::async_write(socket_,
                                     boost::asio::buffer(respstr.data(), respstr.size()),
                                     boost::bind(&ClientSession::handle_write, this,
                                     boost::asio::placeholders::error));
        } else {
            socket_.async_read_some(boost::asio::buffer(client_request_parser_.data_.data(), REQUEST_BUFFER_SIZE),
                                    boost::bind(&ClientSession::handle_read, this,
                                    boost::asio::placeholders::error,
                                    boost::asio::placeholders::bytes_transferred));
        }
    } else {
        //error on read. Usually this means that the client disconnected, so
        //i destroy the client session object
        //std::cout << "error num:" << error.value() << ", error desc:" << error.message() << std::endl;
        delete this;
    }
} //void ClientSession::handle_read


//
void ClientSession::handle_write(const boost::system::error_code& error)
{
    if (!error){

        socket_.async_read_some(boost::asio::buffer(client_request_parser_.data_.data(), REQUEST_BUFFER_SIZE),
                                boost::bind(&ClientSession::handle_read, this,
                                boost::asio::placeholders::error,
                                boost::asio::placeholders::bytes_transferred));

    } else {
        delete this;
    }
}//void ClientSession::handle_write
