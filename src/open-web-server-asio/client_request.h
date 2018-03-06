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
    std::string request_body;
    std::string hostname;
    std::string cookie;
    std::string user_agent;
    std::string accept;
    std::string accept_charset;
    std::string accept_encoding;
    std::string accept_language;
    http_method method;
    http_protocol_version http_protocol_ver;
    //char http_protocol_ver_major; //0-9
    //char http_protocol_ver_minor; //0-9
    uint64_t content_length = 0;
    http_connection connection = http_connection::unknown;

    bool is_range_request = false;
    unsigned long long int range_from_byte;
    unsigned long long int range_until_byte;

    int parser_content_begin_index;

    ClientResponse response;
    std::unordered_map<CacheKey, CacheContent>::iterator cache_iterator;

    std::string hostname_and_uri;//ypologizetai otan ginei parse to uri kai to hostname

    // parser specific
    const int REQUEST_BUFFER_SIZE = 1024;//default 1024
    std::vector<char> buffer;
    http_parser_state parser_state = http_parser_state::state_start;
    //http_parser_state parser_last_known_state;
    //size_t parser_current_state_index = 0;
    //size_t parser_previous_state_index = 0;
    size_t buffer_position = 0;
    size_t total_bytes_transfered = 0;
    static http_parser_result parse(ClientRequest &cr, size_t bytes_transferred);
    void cleanup();
private:
};

#endif // CLIENT_REQUEST_H
