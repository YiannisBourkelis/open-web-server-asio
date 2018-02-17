#ifndef CLIENT_REQUEST_H
#define CLIENT_REQUEST_H

#include <QStringRef>

class ClientRequest
{
public:
    ClientRequest();

    QString raw_request;
    QStringRef uri;
    QStringRef hostname;
};

#endif // CLIENT_REQUEST_H
