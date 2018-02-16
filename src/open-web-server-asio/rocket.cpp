#include "rocket.h"
#include "asio_server.h"

rocket::rocket()
{

}

void rocket::takeoff()
{
    //starts the asynchronous server on port 12344
    //end waits for new clients to connect
    AsioServer asio_server(this->io_service_, 12344);
    this->io_service_.run();
}
