#include "rocket.h"
#include "server_config_parser_base.h"
#include "server_config_json_parser.h"
#include "server_config.h"
#include <ctime>

//public statics
boost::asio::io_service rocket::io_service;
Cache rocket::cache(rocket::io_service);

//private statics
std::chrono::time_point<std::chrono::high_resolution_clock> rocket::gmt_date_time_last_;
std::string rocket::gmt_date_time_;
unsigned long long rocket::etag_ = 0;

//class implementation
rocket::rocket()
{
     gmt_date_time_last_ = std::chrono::high_resolution_clock::now();
}

void rocket::takeoff(QCoreApplication *qcore_aplication)
{
    //load the server config file.
    //This file contains all the server settings regarding
    //virtual hosts, index pages, cache size etc.

    ServerConfig::initialize();//init the server config.

    ServerConfigJsonParser server_config_json_parser;//this will parse the default server_config.json file.
    ServerConfig::server_config_parser = &server_config_json_parser;

    //parse the config file and listen to the ports specified in the server config file
    ServerConfig::io_service_ = &rocket::io_service;
    ServerConfig::parse_config_file(ServerConfig::config_file_path);

    rocket::cache.initialize(qcore_aplication);

    rocket::io_service.run();
}

//get the current date time for use in the http response Date header.
//The time is cached for 1 sec for performance reasons.
//Getting and formatting a date without caching is expensive in CPU.
const std::string &rocket::get_gmt_date_time(std::time_t &time_now)
{
    if (std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::high_resolution_clock::now() - gmt_date_time_last_).count() > 1000)
    {
        //get and format the date and time only if 1sec from the last time has passed.
        time_now = std::time(nullptr);
        tm * tm_ = gmtime(&time_now);
        char char_time[29];
        strftime(char_time, 29, "%a, %d %b %Y %T GMT", tm_);
        std::string string_time(char_time,29);
        gmt_date_time_ = string_time;
        gmt_date_time_last_ = std::chrono::high_resolution_clock::now();
    }
    return gmt_date_time_;
}
/*
const std::string &rocket::get_gmt_date_time(std::time_t &time_now)
{
    if (std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::high_resolution_clock::now() - gmt_date_time_last_).count() > 1000)
    {
        //get and format the date and time only if 1sec from the last time has passed.
        time_now = std::time(nullptr);
        std::stringstream ss_time;
        ss_time << std::put_time(std::gmtime(&time_now), "%a, %d %b %Y %T GMT"); //e.g.: Sat, 20 Jan 2018 09:34:52 GMT
        std::string string_time(ss_time.str());
        gmt_date_time_ = string_time;
        //gmt_date_time_ = QDateTime::currentDateTimeUtc().toString("ddd, dd MMM yyyy hh:mm:ss").toStdString() + " GMT"; //slow
        gmt_date_time_last_ = std::chrono::high_resolution_clock::now();
    }
    return gmt_date_time_;
}
*/

const std::string rocket::get_next_etag()
{
#ifdef WIN32
    if(etag_ == _UI64_MAX){
#else
    if(etag_ == ULONG_LONG_MAX){
#endif
        etag_ = 0;
    }
    return std::to_string(++etag_);
}
