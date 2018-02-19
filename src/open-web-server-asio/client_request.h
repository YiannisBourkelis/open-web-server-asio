#ifndef CLIENT_REQUEST_H
#define CLIENT_REQUEST_H

#include <QStringRef>
#include "client_response.h"

class ClientRequest
{
public:
    ClientRequest();

    QString raw_request;
    QString uri;
    QString hostname;

    ClientResponse response;
};

#endif // CLIENT_REQUEST_H
