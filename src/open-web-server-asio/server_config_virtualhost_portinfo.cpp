#include "server_config_virtualhost_portinfo.h"

ServerConfigVirtualHostPortInfo::ServerConfigVirtualHostPortInfo(io_service &io_service__, short port)
{
    //TODO: should check this line bellow. Maybe it leaks memory
    asio_server = new AsioServerBase(io_service__, port);
}
