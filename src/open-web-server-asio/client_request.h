#ifndef CLIENT_REQUEST_H
#define CLIENT_REQUEST_H

#include "client_response.h"
#include <unordered_map>
#include "cache_key.h"
#include "cache_content.h"
#include <string>
#include "vector"
#include <QByteArray>
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
    http_connection connection;

    bool is_range_request;
    unsigned long long int range_from_byte;
    unsigned long long int range_until_byte;

    int parser_content_begin_index;

    ClientResponse response;
    std::unordered_map<CacheKey, CacheContent>::iterator cache_iterator;

    std::string hostname_and_uri;//ypologizetai otan ginei parse to uri kai to hostname

    // parser specific
    const int REQUEST_BUFFER_SIZE = 1024;
    std::vector<char> crlf_crlf {'\r', '\n', '\r', '\n'};
    std::vector<char> data_;
    QByteArray previous_request_data_;
    static int parse(std::vector<char> &data, size_t bytes_transferred, ClientRequest &client_request);
    bool proccess_new_data(size_t bytes_transferred, ClientRequest &client_request);

private:
};

#endif // CLIENT_REQUEST_H
