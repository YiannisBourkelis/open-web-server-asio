#ifndef CLIENT_SESSION_H
#define CLIENT_SESSION_H

#include <cstdlib>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>

#include <QByteArray>
#include <QString>
#include <QFile>
#include "client_request.h"
#include "client_request_parser.h"
#include "client_response.h"
#include <QMimeDatabase>

using namespace boost::asio;

class ClientSession
{
public:
    ClientSession (boost::asio::io_service& io_service, boost::asio::ssl::context& context, bool is_encrypted_session);

    //methods
    ip::tcp::socket &socket();
    boost::asio::ssl::stream<boost::asio::ip::tcp::socket>::lowest_layer_type& ssl_socket();
    void start();
private:
    //variables
    ip::tcp::socket socket_;
    boost::asio::ssl::stream<boost::asio::ip::tcp::socket> ssl_socket_;
    bool is_encrypted_session_;

    const int REQUEST_BUFFER_SIZE = 1024;
    static const QMimeDatabase mime_db_;

    //xwrizei to arxeio pou zitithike se tmimata isa me ta bytes pou orizontai.
    //Afora arxeia pou den mporoun na mpoun stin cache
    static const int FILE_CHUNK_SIZE = 32768; //default=32768 8192=8.1mb/s, 16384/32768=8.5mb/s 65536 131072 262144 524288 >1048576 2097152 4194304


    ClientRequest client_request_;
    ClientRequestParser client_request_parser_;
    //ClientResponseGenerator client_response_generator_;


    //methods
    void handle_read(const boost::system::error_code &error, size_t bytes_transferred);
    void handle_write(const boost::system::error_code &error);
    void process_client_request();
    void read_and_send_requested_file(QFile &file_io);
    bool try_get_request_uri_resource(QFile &file_io);
    static bool is_malicious_path(QString &path);
    bool add_to_cache_if_fits(QFile &file_io);
    void send_file_from_cache();
    void send_404_not_found_response();
    bool try_send_directory_listing();
    void send_400_bad_request_response();
    void handle_handshake(const boost::system::error_code &error);
};

#endif // CLIENT_SESSION_H
