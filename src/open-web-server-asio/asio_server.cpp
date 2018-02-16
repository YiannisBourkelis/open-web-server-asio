#include "asio_server.h"

//constructor. Starts the server and begins accepting connections
AsioServer::AsioServer(boost::asio::io_service& io_service, short port) :
    io_service_(io_service),
    acceptor_(io_service, ip::tcp::endpoint(ip::tcp::v4(), port))
{
    //disable naggle algorithm
    ip::tcp::no_delay no_delay_option_(true);
    acceptor_.set_option(no_delay_option_);

    //create a client session object where we will handle the communication with the client.
    ClientSession *new_session = new ClientSession(this->io_service_); //= new ClientSession(io_service_);
    //When a new client connects, call AsioServer::handle_accept accept
    //so that we can accept the new clients and begin communicatinf with him.
    //Notice that we pass the new_session object we created.
    acceptor_.async_accept(new_session->socket(),
                           boost::bind(&AsioServer::handle_accept, this, new_session,
                           boost::asio::placeholders::error));

    //m_acceptor.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true))
}

//async callback: called each time a new client connects
void AsioServer::handle_accept(ClientSession* new_session, const boost::system::error_code& error)
{
    if (!error) {
        //if there is no error the client is accepted.

        //disable the naggle algorithm
        ip::tcp::no_delay no_delay_option_(true);
        new_session->socket().set_option(no_delay_option_);


        //**** starts recieving data inside the client session object.
        new_session->start();
        //****

        //Create a new client session for the next client
        //when it will connect, and provide the callback function to accept the connection.
        new_session = new ClientSession(io_service_);
        acceptor_.async_accept(new_session->socket(),
                               boost::bind(&AsioServer::handle_accept, this, new_session,
                               boost::asio::placeholders::error));
    } else {
        //if there is an error on accept delete the client session.
        delete new_session;
    }
}
