#ifndef CLIENT_SESSION_H
#define CLIENT_SESSION_H

#include <cstdlib>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <QByteArray>
#include <QString>
#include <QFile>
#include "client_request.h"
#include "client_request_parser.h"
//#include "client_response_generator.h"
#include "client_response.h"
#include <QMimeDatabase>
//#include "server_config.h"

using namespace boost::asio;

class ClientSession
{
public:
    ClientSession (boost::asio::io_service& io_service);

    //methods
    ip::tcp::socket &socket();
    void start();
private:
    //variables
    ip::tcp::socket socket_;
    const int REQUEST_BUFFER_SIZE = 1024;
    static const QMimeDatabase mime_db_;

    //xwrizei to arxeio pou zitithike se tmimata isa me ta bytes pou orizontai.
    //Afora arxeia pou den mporoun na mpoun stin cache
    static const int FILE_CHUNK_SIZE = 32768; //8192=8.1mb/s, 16384/32768=8.5mb/s 65536 131072 262144 524288 >1048576 2097152 4194304


    ClientRequest client_request_;
    ClientRequestParser client_request_parser_;
    //ClientResponseGenerator client_response_generator_;


    //methods
    void handle_read(const boost::system::error_code &error, size_t bytes_transferred);
    void handle_write(const boost::system::error_code &error);
    void process_client_request();
    void read_requested_file(QFile &file_io);
    bool try_get_request_uri_resource(QFile &file_io);
    static bool is_malicious_path(QString &path);
};

#endif // CLIENT_SESSION_H
