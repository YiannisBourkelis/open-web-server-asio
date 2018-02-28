#include "asio_server_base.h"
#include "client_session_encrypted.h"
#include "client_session_plain.h"

//constructor. Starts the server and begins accepting connections
AsioServerBase::AsioServerBase(io_service &io_service, short port) :
    io_service_(io_service),
    acceptor_(io_service, ip::tcp::endpoint(ip::tcp::v4(), port))
{
}

AsioServerBase::~AsioServerBase()
{

}
