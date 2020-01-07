#include "rocket.h"
#include "server_config_parser_base.h"
#include "server_config_json_parser.h"
#include "server_config.h"
#include <ctime>
#include <QLocale>
#include <boost/thread.hpp>

//public statics
boost::asio::io_service rocket::io_service;
Cache rocket::cache(rocket::io_service);
QLocale rocket::en_us_locale;
const QMimeDatabase rocket::mime_db_;
const int rocket::FILE_CHUNK_SIZE;
const int rocket::REQUEST_BUFFER_SIZE;

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
    //required to set the locale to "C", to get
    //locale independant date/time formats in strftime formating
    setlocale (LC_ALL,"C");

    //also QDate formating allways get the system locale to
    //format dates, so I create an en_US locale to allways have
    //the same date format
    rocket::en_us_locale = QLocale(QLocale::English, QLocale::UnitedStates);

    //load the server config file.
    //This file contains all the server settings regarding
    //virtual hosts, index pages, cache size etc.
    ServerConfig::initialize();//init the server config.

    ServerConfigJsonParser server_config_json_parser;//this will parse the default server_config.json file.
    ServerConfig::server_config_parser = &server_config_json_parser;

    //parse the config file and listen to the ports specified in the server config file
    ServerConfig::io_service_ = &rocket::io_service;
    ServerConfig::parse_config_file(ServerConfig::config_file_path);

    //initilizes the filesystem watcher, so in case of a file change
    //the cache will be updated, if this file exist in the cach
    rocket::cache.initialize(qcore_aplication);

    std::cout << OPENSSL_VERSION_TEXT << std::endl;
    std::cout << "Boost lib version: " BOOST_LIB_VERSION << std::endl;



    boost::thread_group tg;
    std::cout << "Launching " << boost::thread::hardware_concurrency() << " worker threads..." << std::endl;
    for (unsigned i = 0; i < boost::thread::hardware_concurrency(); ++i)
    //for (unsigned i = 0; i < 4; ++i)
    {
        auto t = tg.create_thread(boost::bind(&io_service::run, &rocket::io_service));
        std::cout << t->get_id() << std::endl;
    }
    std::cout << "Worker threads created successfully." << std::endl;
    std::cout << "Web server is up and running!" << std::endl;
    tg.join_all();

    //rocket::io_service.run();
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
        strftime(char_time, 30, "%a, %d %b %Y %T GMT", tm_);
        std::string string_time(char_time,29);
        gmt_date_time_ = string_time;
        gmt_date_time_last_ = std::chrono::high_resolution_clock::now();
    }
    return gmt_date_time_;
}

//A very efficient way to get an etag for versioning each
//element in the cache. The risk of a dublicate is close to 0,
//with a smaller probability than using a fast hashing algorithm,
//and of course using fewer CPU cycles.
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
