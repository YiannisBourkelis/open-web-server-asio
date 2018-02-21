#ifndef CLIENT_RESPONSE_H
#define CLIENT_RESPONSE_H

#include <unordered_map>
#include "qstring_hash_specialization.h"
#include "server_config_virtualhost.h"
#include <vector>

class ClientResponse
{
public:
    ClientResponse();

    enum class state{
        begin,
        single_send,
        chunk_send,
        cache_header_send
    };

    state current_state = state::begin;//at which state of the response we are
    QString absolute_hostname_and_requested_path;//the file path requested as it appears in the filesystem
    std::unordered_map<QString, ServerConfigVirtualHost>::iterator server_config_map_it;
    std::string header;//otan ginetai apostoli apo tin cache, apothikevetai edw to header pros apostoli
    std::vector<char> data;//the response data to send to the client if the recource is not in the cache;
    long long bytes_of_file_sent;
};

#endif // CLIENT_RESPONSE_H
