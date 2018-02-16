#include <QCoreApplication>
#include "rocket.h"

//run as windows service
//https://nssm.cc
//https://www.rozanski.org.uk/software

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    rocket rocket_;
    rocket_.takeoff();

    return a.exec();
}

//https://en.wikipedia.org/wiki/Proactor_pattern
