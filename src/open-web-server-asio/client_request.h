#ifndef CLIENT_REQUEST_H
#define CLIENT_REQUEST_H

#include "client_response.h"
#include <unordered_map>
#include "cache_key.h"
#include "cache_content.h"
#include <string>
#include "vector"
#include "parser_enums.h"

enum class http_protocol_version {
    HTTP_2_0,
    HTTP_1_1,
    HTTP_1_0,
    HTTP_0_9
};

enum class http_connection {
    unknown,
    keep_alive,
    close
};

enum class http_method {
    GET,
    POST,
    HEAD
};

class ClientRequest
{
public:
    ClientRequest();

    std::vector<char> raw_request;
    std::string uri;
    std::string query_string;
    std::string hostname;
    http_method method;
    http_protocol_version http_protocol_ver;
    size_t content_size = 0;
    http_connection connection = http_connection::unknown;

    bool is_range_request;
    unsigned long long int range_from_byte;
    unsigned long long int range_until_byte;

    int parser_content_begin_index;

    ClientResponse response;
    std::unordered_map<CacheKey, CacheContent>::iterator cache_iterator;

    std::string hostname_and_uri;//ypologizetai otan ginei parse to uri kai to hostname

    // parser specific
    const int REQUEST_BUFFER_SIZE = 1024;
    std::vector<char> data;
    http_parser_state parser_current_state = start_state;
    size_t parser_current_state_index = 0;
    static http_parser_result parse(ClientRequest &cr, size_t bytes_transferred);
    void cleanup();
private:
};

#endif // CLIENT_REQUEST_H
