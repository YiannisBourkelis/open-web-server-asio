#ifndef NO_ENCRYPTION
#include "client_session_encrypted.h"

ClientSessionEncrypted::ClientSessionEncrypted(boost::asio::io_service& io_service, boost::asio::ssl::context& context) :
    ClientSessionBase(io_service),
    ssl_socket_(io_service, context)
{
}

ClientSessionEncrypted::~ClientSessionEncrypted()
{
}

boost::asio::ssl::stream<boost::asio::ip::tcp::socket>::lowest_layer_type& ClientSessionEncrypted::ssl_socket()
{
  return ssl_socket_.lowest_layer();
}

void ClientSessionEncrypted::close_socket()
{
    this->ssl_socket().close();
}

void ClientSessionEncrypted::start()
{
    ssl_socket_.async_handshake(boost::asio::ssl::stream_base::server,
                                boost::bind(&ClientSessionEncrypted::handle_handshake, this,
                                boost::asio::placeholders::error));
}

void ClientSessionEncrypted::handle_handshake(const boost::system::error_code& error)
{
  if (!error)
  {
      async_read_some(client_request_.data_);
  }
  else
  {
     //TODO: have to figure out what to do whith the handshake error. Not important at the moment.
    std::cout << error.message() << "  Error Value: " << error.value() << std::endl;
    if (this->ssl_socket().lowest_layer().is_open()){
        this->ssl_socket().lowest_layer().close();
    }
    delete this;
  }
}

void ClientSessionEncrypted::async_read_some(std::vector<char> &buffer)
{
    ssl_socket_.async_read_some(boost::asio::buffer(buffer.data(), buffer.size()),
                            boost::bind(&ClientSessionEncrypted::handle_read, this,
                            boost::asio::placeholders::error,
                            boost::asio::placeholders::bytes_transferred));
}

void ClientSessionEncrypted::async_write(std::vector<boost::asio::const_buffer> &buffers)
{
    boost::asio::async_write(ssl_socket_,
                             buffers,
                             boost::bind(&ClientSessionEncrypted::handle_write, this,
                             boost::asio::placeholders::error));
}

void ClientSessionEncrypted::async_write(std::vector<char> &buffer)
{
    boost::asio::async_write(ssl_socket_,
                             boost::asio::buffer(buffer.data(), buffer.size()),
                             boost::bind(&ClientSessionEncrypted::handle_write, this,
                             boost::asio::placeholders::error));
}
#endif //#ifndef NO_ENCRYPTION
