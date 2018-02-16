#ifndef ROCKET_H
#define ROCKET_H

#include <boost/asio.hpp>

using namespace boost::asio;

class rocket
{
public:
    rocket();

    void takeoff();

private:
    boost::asio::io_service io_service_;
};

#endif // ROCKET_H
