#include "server_config_virtualhost_portinfo.h"

ServerConfigVirtualHostPortInfo::ServerConfigVirtualHostPortInfo(io_service &io_service__, short port, bool is_encrypted_server)
{
    asio_server = new AsioServer(io_service__, port, is_encrypted_server);
}
