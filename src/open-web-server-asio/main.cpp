#include <QCoreApplication>
#include "rocket.h"
#include "qt_event_loop_init.h"
#include <QFileSystemWatcher>


//run as windows service
//https://nssm.cc
//https://www.rozanski.org.uk/software

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    //QCoreApplication::sendPostedEvents();

    //QFileSystemWatcher q(&a);

    rocket rocket_;
    rocket_.takeoff();

    return a.exec();
}

//https://en.wikipedia.org/wiki/Proactor_pattern

/* folder hierarchy
 * /bin       <- executable file, libraries etc
 * /config   <- server config file(s)
 * GNUGPL    <- license file
 * other lic <- 3rd party libraries licences
 */
