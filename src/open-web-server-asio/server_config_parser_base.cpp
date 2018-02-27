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

#include "server_config_parser_base.h"

ServerConfigParserBase::ServerConfigParserBase()
{

}

ServerConfigParserBase::~ServerConfigParserBase()
{

}

bool ServerConfigParserBase::parse_config_file(const QString &filename,
                                               std::unordered_map<QString, ServerConfigVirtualHost> &server_config_map,
                                               std::unordered_map<short, AsioServer *> &server_open_ports,
                                               io_service &io_service_)
{
    Q_UNUSED(filename);
    Q_UNUSED(server_config_map);
    Q_UNUSED(server_open_ports);
    Q_UNUSED(io_service_);
    return false;
}
