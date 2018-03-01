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

#include "server_config_json_parser.h"
#include <QFile>
#include <iostream>
#include <fstream>
#include "json.hpp"
#include <QCoreApplication>

ServerConfigJsonParser::ServerConfigJsonParser()
{

}

ServerConfigJsonParser::~ServerConfigJsonParser()
{

}

bool ServerConfigJsonParser::parse_config_file(const QString &filename,
                                               std::unordered_map<QString, ServerConfigVirtualHost> &server_config_map,
                                               std::unordered_map<short, AsioServerBase*> &server_open_ports,
                                               boost::asio::io_service &io_service_)
{
    std::ifstream i(filename.toStdString());
    std::cout << "Trying to load the server configuration file at: " << filename.toStdString() << "\r\n";
    if (!i.is_open()){
        std::cout << "Could not find server_config.json at: " << filename.toStdString() << "\r\n";
        std::cout << "Exiting...\r\n";
        QCoreApplication::exit();
        return false;
    }
    std::cout << "Server config file found. Begin parsing...\r\n";

    server_config_map.clear();

    nlohmann::json j;
    i >> j;

    auto http_settings_ = j["http"];
    if (http_settings_.is_null()) {
        std::cout << "ERROR: Server config parsing: \"http\" section not found\r\n";
        return false;
    }

    for (auto http_setting : http_settings_){
        auto vhost_hostnames = http_setting["virtual_host"]["server_name"];

        for (auto listen__ : http_setting["virtual_host"]["listen"]){
            for (size_t vhost_hostnames_idx = 0; vhost_hostnames_idx < vhost_hostnames.size(); vhost_hostnames_idx++){
                bool is_encrypted_server = false;
                if (listen__["encryption"].is_boolean()){
                    is_encrypted_server = listen__["encryption"].get<bool>();
                }

                int port_ = listen__["port"].get<short>();
                if (server_open_ports.find(port_) == server_open_ports.end()){
                    /******* HERE A NEW SERVER IS CREATED AND IS LISTENING FOR INCOMMING CONNECTIONS ****/
                    if (is_encrypted_server){
#ifndef NO_ENCRYPTION
                        server_open_ports.insert(std::make_pair(port_, new AsioServerEncrypted(io_service_, port_)));
#endif
                    } else {
                        server_open_ports.insert(std::make_pair(port_, new AsioServerPlain(io_service_, port_)));
                    }
                }

                QString port__ = QString::number(port_);
                //QString port__ = QString::fromStdString(listen__["port"].get<std::string>());

                ServerConfigVirtualHost vhost;
                QString vhost_name = QString::fromStdString(vhost_hostnames.at(vhost_hostnames_idx).get<std::string>());
                vhost.ServerName.push_back(vhost_name);
                vhost.DocumentRoot = QString::fromStdString(http_setting["virtual_host"]["document_root"].get<std::string>());
                vhost.allow_directory_listing = http_setting["virtual_host"]["directory_listing"].get<bool>();

                //directory indexes
                for (auto idx_itm : j["server"]["indexes"]){
                    vhost.directoryIndexes.push_back(QString::fromStdString(idx_itm.get<std::string>()));
                }

                //kataxwrisi tou vhost sto unordered map
                QString server_and_port = vhost_name != "*" ? vhost_name + ":" + port__ : vhost_name;
                server_config_map.emplace(std::make_pair(server_and_port, vhost));
            }

        }//for listen_array
    }//for http_settings

    //AsioServer *a = new AsioServer(io_service_, 12343);

    return true;
}
