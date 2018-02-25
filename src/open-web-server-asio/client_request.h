#ifndef CLIENT_REQUEST_H
#define CLIENT_REQUEST_H

#include "client_response.h"
#include <unordered_map>
#include "cache_key.h"
#include "cache_content.h"
#include <QStringRef>

class ClientRequest
{
public:
    ClientRequest();

    QString raw_request;
    QString uri;
    QStringRef uri_ref;
    QStringRef host_ref;
    QString hostname;

    bool is_range_request;
    unsigned long long int range_from_byte;
    unsigned long long int range_until_byte;

    int parser_content_begin_index;
    //int parser_content_end_index;

    ClientResponse response;
    std::unordered_map<CacheKey, CacheContent>::iterator cache_iterator;

    QString hostname_and_uri;//ypologizetai otan ginei parse to uri kai to hostname
private:
};

#endif // CLIENT_REQUEST_H
