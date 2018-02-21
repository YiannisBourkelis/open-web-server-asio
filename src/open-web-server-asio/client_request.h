#ifndef CLIENT_REQUEST_H
#define CLIENT_REQUEST_H

#include "client_response.h"
#include <unordered_map>
#include "cache_key.h"
#include "cache_content.h"

class ClientRequest
{
public:
    ClientRequest();

    QString raw_request;
    QString uri;
    QString hostname;

    ClientResponse response;
    std::unordered_map<CacheKey, CacheContent>::iterator cache_iterator;

    QString hostname_and_uri;//ypologizetai otan ginei parse to uri kai to hostname
private:
};

#endif // CLIENT_REQUEST_H
