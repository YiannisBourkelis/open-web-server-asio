#ifndef CLIENT_REQUEST_PARSER_H
#define CLIENT_REQUEST_PARSER_H

#include <QByteArray>
#include "client_request.h"
#include <QStringRef>
#include <vector>
#include "parser_enums.h"

class ClientRequestParser
{
public:
    ClientRequestParser();
    static int parse(std::vector<char> &data, size_t bytes_transferred, ClientRequest &client_request);
    bool proccess_new_data(size_t bytes_transferred, ClientRequest &client_request);

    const int REQUEST_BUFFER_SIZE = 1024;
    std::vector<char> crlf_crlf {'\r', '\n', '\r', '\n'};
    std::vector<char> data_;
    QByteArray previous_request_data_;
    int parse2(std::vector<char> &data, size_t bytes_transferred, ClientRequest &client_request);
};

#endif // CLIENT_REQUEST_PARSER_H
