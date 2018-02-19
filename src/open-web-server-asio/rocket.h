#ifndef ROCKET_H
#define ROCKET_H

#include <boost/asio.hpp>
#include <unordered_map>
#include <asio_server.h>

using namespace boost::asio;

class rocket
{
public:
    rocket();

    void takeoff();
    std::unordered_map<short, AsioServer*> servers_;

private:
    boost::asio::io_service io_service_;
};

#endif // ROCKET_H
