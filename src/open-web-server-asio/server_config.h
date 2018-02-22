/* ***********************************************************************
 * Open Web Server
 * (C) 2018 by Yiannis Bourkelis (hello@andama.org)
 *
 * This file is part of Open Web Server.
 *
 * Open Web Server is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Open Web Server is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Open Web Server.  If not, see <http://www.gnu.org/licenses/>.
 * ***********************************************************************/

#ifndef SERVER_CONFIG_H
#define SERVER_CONFIG_H

#include "server_config_virtualhost.h"
#include "server_config_parser_base.h"
#include "qstring_hash_specialization.h"
#include <QString>
#include <QStringRef>
#include <QFile>
#include <QCoreApplication>
#include "client_request.h"
#include "server_config_virtualhost_portinfo.h"
#include <boost/asio.hpp>
#include "asio_server.h"

class ServerConfig
{
public:
    ServerConfig();

    //variables
    static boost::asio::io_service *io_service_;

    //contains the virtual hosts
    static std::unordered_map<QString, ServerConfigVirtualHost> server_config_map;

    //contains the ports required to listen for incomming connections
    //static std::unordered_map<short, ServerConfigVirtualHostPortInfo> server_open_ports;
    static std::unordered_map<short, AsioServer*> server_open_ports;

    static ServerConfigParserBase *server_config_parser;
    static QString application_path; //the path from the web server executable runs
    static QString config_file_path; //by default the path is one level under the application_path, at the config folder

    //methods
    static bool parse_config_file(QString filename);
    static bool is_valid_requested_hostname(ClientRequest &client_request);
    static bool index_exists(ClientRequest &client_request, QFile & file_io);
    //static AsioServer create_asio_server(boost::asio::io_service &io_service, short port);
    static void initialize();
};

#endif // SERVER_CONFIG_H
