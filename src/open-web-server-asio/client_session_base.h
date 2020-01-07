#ifndef CLIENT_SESSION_BASE_H
#define CLIENT_SESSION_BASE_H

#include <cstdlib>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/asio/buffer.hpp>
#include <QByteArray>
#include <QString>
#include <QFile>
#include <QMimeDatabase>
#include <unordered_map>
#include "client_request.h"
#include "client_response.h"

using namespace boost::asio;

class ClientSessionBase
{
public:
    ClientSessionBase (boost::asio::io_service& io_service);
    virtual ~ClientSessionBase();

    //methods
    void start();
    virtual void async_read_some(std::vector<char> &buffer);
    virtual void async_read_some(std::vector<char> &buffer, size_t begin_offset, size_t max_size);
    virtual void async_write(std::vector<boost::asio::const_buffer> &buffers);
    virtual void async_write(std::vector<char> &buffer);
    virtual void close_socket();

protected:
    //variables
    ClientRequest client_request_;

    //methods
    void handle_read(const boost::system::error_code &error, size_t bytes_transferred);
    void handle_write(const boost::system::error_code &error);

private:
    //variables
    boost::asio::io_service &io_service_;


    //methods
    void process_client_request();
    void read_and_send_requested_file(QFile &file_io);
    bool try_open_request_uri_resource(QFile &file_io);
    static bool is_malicious_path(QString &path);
    bool add_to_cache_if_fits(QFile &file_io);
    void send_file_from_cache(std::unordered_map<CacheKey, CacheContent>::iterator &cache_iterator);
    void send_404_not_found_response();
    bool try_send_directory_listing();
    void send_400_bad_request_response();
    void handle_handshake(const boost::system::error_code &error);
};

#endif // CLIENT_SESSION_H
