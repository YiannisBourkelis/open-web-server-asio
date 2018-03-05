#ifndef CLIENT_RESPONSE_H
#define CLIENT_RESPONSE_H

#include <unordered_map>
#include "qstring_hash_specialization.h"
#include "server_config_virtualhost.h"
#include <vector>

#include "parser_enums.h"
using namespace enums;
typedef http_parser_state s;

class ClientResponse
{
public:
    ClientResponse();

    enum class state{
        begin,
        single_send,
        chunk_send
    };

    state current_state = state::begin;//at which state of the response we are
    QString absolute_hostname_and_requested_path;//the file path requested as it appears in the filesystem
    std::unordered_map<QString, ServerConfigVirtualHost>::iterator server_config_map_it;
    std::string header;//otan ginetai apostoli apo tin cache, apothikevetai edw to header pros apostoli
    std::vector<char> data;//the response data to send to the client if the recource is not in the cache;
    long long bytes_of_file_sent;

    //std::vector<char> data_in;//the response data generated by cgi/fcgi;
    size_t data_in_body_size;
    size_t data_in_header_size;
    std::vector<char> header_in;
    std::vector<char> body_in;
    std::vector<char> data_in;
    //std::string body;

    static int parse(std::vector<char> &response_data, size_t bytes_transferred, ClientResponse &client_response);
};


#endif // CLIENT_RESPONSE_H
