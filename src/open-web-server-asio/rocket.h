#ifndef ROCKET_H
#define ROCKET_H

#include <boost/asio.hpp>
#include <unordered_map>
#include "asio_server_base.h"
#include "cache.h"
#include <string>
#include <chrono>
#include <QCoreApplication>
#include <QLocale>

using namespace boost::asio;

class rocket
{
public:
    rocket();

    static boost::asio::io_service io_service;

    void takeoff(QCoreApplication *qcore_aplication);
    std::unordered_map<short, AsioServerBase*> servers_;
    static Cache cache;
    static QLocale en_us_locale;
    static const QMimeDatabase mime_db_;

    //xwrizei to arxeio pou zitithike se tmimata isa me ta bytes pou orizontai.
    //Afora arxeia pou den mporoun na mpoun stin cache
    static const int FILE_CHUNK_SIZE = 32768; //default=32768 8192=8.1mb/s, 16384/32768=8.5mb/s 65536 131072 262144 524288 >1048576 2097152 4194304
    static const int REQUEST_BUFFER_SIZE = 1024;


    static const std::string &get_gmt_date_time(std::time_t &time_now);
    static const std::string get_next_etag();
private:

    static std::chrono::time_point<std::chrono::high_resolution_clock> gmt_date_time_last_;
    static std::string gmt_date_time_;

#ifdef WIN32
    static unsigned __int64 etag_;
#else
    static unsigned long long etag_;
#endif
};

#endif // ROCKET_H
