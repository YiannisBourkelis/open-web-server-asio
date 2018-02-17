#ifndef CLIENT_REQUEST_PARSER_H
#define CLIENT_REQUEST_PARSER_H

#include <QByteArray>
#include <client_request.h>

class ClientRequestParser
{
public:
    ClientRequestParser();
    static int parse(QByteArray &data, ClientRequest &client_request);
    bool proccess_new_data(size_t bytes_transferred, ClientRequest &client_request);

    const int REQUEST_BUFFER_SIZE = 1024;
    const QByteArray crlf_crlf= "\r\n\r\n";
    QByteArray data_;
    QByteArray previous_request_data_;
};

#endif // CLIENT_REQUEST_PARSER_H
