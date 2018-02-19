#include "server_config_virtualhost_portinfo.h"

ServerConfigVirtualHostPortInfo::ServerConfigVirtualHostPortInfo(io_service &io_service__, short port)
{
    asio_server = new AsioServer(io_service__, port);
}
