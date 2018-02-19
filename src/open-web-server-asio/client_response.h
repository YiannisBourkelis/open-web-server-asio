#ifndef CLIENT_RESPONSE_H
#define CLIENT_RESPONSE_H

#include <unordered_map>
#include "qstring_hash_specialization.h"
#include "server_config_virtualhost.h"

class ClientResponse
{
public:
    ClientResponse();

    enum class state{
        begin,
        single_send,
        chunk_send
    };

    state current_state = state::begin;
    QString absolute_hostname_and_requested_path;
    std::unordered_map<QString, ServerConfigVirtualHost>::iterator server_config_map_it;
};

#endif // CLIENT_RESPONSE_H
