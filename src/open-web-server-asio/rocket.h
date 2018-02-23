#ifndef ROCKET_H
#define ROCKET_H

#include <boost/asio.hpp>
#include <unordered_map>
#include "asio_server.h"
#include "cache.h"
#include <string>
#include <chrono>
#include <QCoreApplication>

using namespace boost::asio;

class rocket
{
public:
    rocket();

    static boost::asio::io_service io_service;

    void takeoff(QCoreApplication *qcore_aplication);
    std::unordered_map<short, AsioServer*> servers_;
    static Cache cache;

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
