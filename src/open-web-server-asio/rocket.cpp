#include "rocket.h"
//#include "asio_server.h"
#include "server_config_json_parser.h"
#include "server_config.h"

Cache rocket::cache;

rocket::rocket()
{

}

void rocket::takeoff()
{
    //load the server config file.
    //This file contains all the server settings regarding
    //virtual hosts, index pages, cache size etc.

    ServerConfig::ServerConfig();//init the server config.

    ServerConfigJsonParser server_config_json_parser;//this will parse the default server_config.json file.
    ServerConfig::server_config_parser = &server_config_json_parser;

    //parse the config file and listen to the ports specified in the server config file
    ServerConfig::io_service_ = &this->io_service_;
    ServerConfig::parse_config_file(ServerConfig::config_file_path);

    this->io_service_.run();
}
