#ifndef SERVER_CONFIG_VIRTUALHOST_PORTINFO_H
#define SERVER_CONFIG_VIRTUALHOST_PORTINFO_H

#include "asio_server.h"
#include <boost/asio.hpp>

class ServerConfigVirtualHostPortInfo
{
public:
    ServerConfigVirtualHostPortInfo(boost::asio::io_service &io_service__, short port);

    int port;
    AsioServer *asio_server;
};

#endif // SERVER_CONFIG_VIRTUALHOST_PORTINFO_H
