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

#include <iostream>
#include "server_config.h"
#include <QStringBuilder>
#include <QDir>
#include <QFileInfo>

boost::asio::io_service *ServerConfig::io_service_;
std::unordered_map<QString, ServerConfigVirtualHost> ServerConfig::server_config_map;
std::unordered_map<short, AsioServerBase*> ServerConfig::server_open_ports;
ServerConfigParserBase *ServerConfig::server_config_parser;
QString ServerConfig::application_path;
QString ServerConfig::config_file_path;

ServerConfig::ServerConfig()
{
}

void ServerConfig::initialize(){
    //ServerConfig::server_config_parser = server_config_parser_base_;

    //store the application path and the config file name in the static variables
    ServerConfig::application_path = QCoreApplication::applicationDirPath();
    QDir tmp_server_config_file_path(ServerConfig::application_path);
    tmp_server_config_file_path.cdUp();
    ServerConfig::config_file_path = QFileInfo(tmp_server_config_file_path.absolutePath() + QString("/config/server_config.json")).filePath();
}

bool ServerConfig::parse_config_file(QString filename)
{
    return server_config_parser->parse_config_file(filename,
                                                   ServerConfig::server_config_map,
                                                   ServerConfig::server_open_ports,
                                                   *ServerConfig::io_service_);
}

bool ServerConfig::is_valid_requested_hostname(ClientRequest &client_request)
{
    //anazitisi ean to hostname yparxei sto map me ta hostnames
    auto it = ServerConfig::server_config_map.find(QString::fromStdString(client_request.hostname));

    if (it != ServerConfig::server_config_map.end()){
        //to hostname vrethike opote lamvanw to path tou kai to ennonw me to
        //path tou resource pou zitithike
        client_request.response.absolute_hostname_and_requested_path = it->second.DocumentRoot % QString::fromStdString(client_request.uri);
        client_request.response.server_config_map_it = std::move(it);
        return true;
    } else {
        //to hostname den vrethike opote elegxw ean yparxei default (*) hostname
        it = ServerConfig::server_config_map.find("*");
        if (it != ServerConfig::server_config_map.end()){
            std::vector<QString> directoryIndexes = it->second.directoryIndexes;
            client_request.response.absolute_hostname_and_requested_path = it->second.DocumentRoot % QString::fromStdString(client_request.uri);
            client_request.response.server_config_map_it = std::move(it);
            return true;
        }
    }

    //den vrethike
    return false;
}

//exontas to virtual host pou zitithike kai to path, psaxnw na
//vrw ean yparxei arxeio index wste na apostalei
bool ServerConfig::index_exists(ClientRequest &client_request, QFile &file_io)
{
    //lamvanw to array me ta indexes apo to iterator pou proekipse
    //kalontas pio prin tin is_valid_requested_hostname
    std::vector<QString> directoryIndexes = client_request.response.server_config_map_it->second.directoryIndexes;

    //krataw ena antigradw tis pliris diadromis pou zitithike, wste na tin epanaferew
    //ean den vrethei index file.
    QString absolute_host_and_uri = client_request.response.absolute_hostname_and_requested_path;
    bool ends_with_slash = client_request.response.absolute_hostname_and_requested_path.endsWith("/");

    //dokimazw kathe index arxeio, prosthetontas to sto yparxon absolute path
    for (const QString &index_filename : directoryIndexes){
        ends_with_slash ? client_request.response.absolute_hostname_and_requested_path += index_filename :
                client_request.response.absolute_hostname_and_requested_path = client_request.response.absolute_hostname_and_requested_path % "/" % index_filename;
        file_io.setFileName(client_request.response.absolute_hostname_and_requested_path);
        if (file_io.open(QFileDevice::ReadOnly) == true) {
            //ean vrethike
                 ends_with_slash ? client_request.uri = client_request.uri + index_filename.toStdString() :
                         client_request.uri = client_request.uri + "/" + index_filename.toStdString();
            return true;
        }
        //epanaferw to absolute path opws itan
        client_request.response.absolute_hostname_and_requested_path = absolute_host_and_uri;
    }

    return false;
}
