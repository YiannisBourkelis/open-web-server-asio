#ifndef NO_ENCRYPTION
#include "asio_server_encrypted.h"

//constructor. Starts the server and begins accepting connections
AsioServerEncrypted::AsioServerEncrypted(io_service &io_service, short port) :
    AsioServerBase(io_service, port),
    context_(boost::asio::ssl::context::tls)
{
    //encryption specific
    context_.set_options(
        boost::asio::ssl::context::default_workarounds);
        //| boost::asio::ssl::context::no_sslv2);
    //context_.set_password_callback(boost::bind(&server::get_password, this));
#ifdef WIN32
   context_.use_certificate_chain_file("C:/Users/Yiannis/Documents/GitHub/poll-echo-client-server/src/certificate.pem");
    context_.use_private_key_file("C:/Users/Yiannis/Documents/GitHub/poll-echo-client-server/src/key.pem", boost::asio::ssl::context::pem);
#else
    context_.use_certificate_chain_file("/Users/yiannis/Projects/poll-echo-client-server-gihub/poll-echo-client-server/src/certificate.pem");
    context_.use_private_key_file("/Users/yiannis/Projects/poll-echo-client-server-gihub/poll-echo-client-server/src/key.pem", boost::asio::ssl::context::pem);
#endif
    //context_.use_tmp_dh_file("dh2048.pem");

    //disable naggle algorithm
    ip::tcp::no_delay no_delay_option_(true);
    acceptor_.set_option(no_delay_option_);

    //create a client session object where we will handle the communication with the client.

    ClientSessionEncrypted *new_session = new ClientSessionEncrypted(this->io_service_, context_); //= new ClientSession(io_service_);

    //When a new client connects, call AsioServer::handle_accept accept
    //so that we can accept the new client and begin communicating with him.
    //Notice that we pass the new_session object we created.
    acceptor_.async_accept(new_session->ssl_socket(),
                           boost::bind(&AsioServerEncrypted::handle_accept, this, new_session,
                           boost::asio::placeholders::error));

    //m_acceptor.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true))
}

AsioServerEncrypted::~AsioServerEncrypted()
{

}


//async callback: called each time a new client connects
void AsioServerEncrypted::handle_accept(ClientSessionEncrypted* new_session, const boost::system::error_code& error)
{
    if (!error) {
        //if there is no error the client is accepted.

        //disable the naggle algorithm
        ip::tcp::no_delay no_delay_option_(true);
        new_session->ssl_socket().set_option(no_delay_option_);


        //**** starts recieving data inside the client session object.
        new_session->start();
        //****


        //Create a new client session for the next client
        //when it will connect, and provide the callback function to accept the connection.
         new_session = new ClientSessionEncrypted(this->io_service_, context_); //= new ClientSession(io_service_);

        //When a new client connects, call AsioServer::handle_accept accept
        //so that we can accept the new client and begin communicating with him.
        //Notice that we pass the new_session object we created.
        acceptor_.async_accept(new_session->ssl_socket(),
                               boost::bind(&AsioServerEncrypted::handle_accept, this, new_session,
                               boost::asio::placeholders::error));

    } else {
        //if there is an error on accept delete the client session.
        delete new_session;
    }
}
#endif //#ifndef NO_ENCRYPTION
