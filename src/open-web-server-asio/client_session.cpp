#include "client_session.h"
#include "server_config.h"
#include <boost/utility/string_view.hpp>
#include "http_response_templates.h"
#include "rocket.h"
#include "cache_content.h"
#include <QStringBuilder>
#include <QDateTime>
#include <time.h>
#include <QFileInfo>
#include <QDir>

const QMimeDatabase ClientSession::mime_db_;
const int ClientSession::FILE_CHUNK_SIZE;

//constructor
ClientSession::ClientSession(boost::asio::io_service& io_service) : socket_(io_service)
{
    //resize the buffer to accept the request.
    //data_.resize(REQUEST_BUFFER_SIZE);
    //client_response_generator_.socket = &this->socket();
}

//returns the active client session socket
ip::tcp::socket& ClientSession::socket()
{
    return socket_;
}

void ClientSession::start()
{
    //Read some data from the client and invoke the callback
    //to proccess the client request
    socket_.async_read_some(boost::asio::buffer(client_request_parser_.data_.data(), REQUEST_BUFFER_SIZE),
                            boost::bind(&ClientSession::handle_read, this,
                            boost::asio::placeholders::error,
                            boost::asio::placeholders::bytes_transferred));
}

void ClientSession::handle_read(const boost::system::error_code& error, size_t bytes_transferred)
{
    if (!error)
    {


        /*
        //benchmarking
        std::string respstr ("HTTP/1.1 200 OK\r\n"
                                   "Content-Type: text/html; charset=utf-8\r\n"
                                   "Content-Length: 5"
                                   "\r\n\r\n"
                                   "Hello");
                                   */


        //first check to see if the data arrived from the client forms a complete
        //http request message (contains or ends with /r/n/r/n).
        if (client_request_parser_.proccess_new_data(bytes_transferred, client_request_)){
            //ok, we have a complete client request. now lets process this request
            //and generate a response to send it to the client
            process_client_request();


            /*
            //benchmarking
            client_request_.response.current_state = ClientResponse::state::single_send;
            boost::asio::async_write(socket_,
                                     boost::asio::buffer(respstr.data(), respstr.size()),
                                     boost::bind(&ClientSession::handle_write, this,
                                     boost::asio::placeholders::error));
                                     */


        } else {
            socket_.async_read_some(boost::asio::buffer(client_request_parser_.data_.data(), REQUEST_BUFFER_SIZE),
                                    boost::bind(&ClientSession::handle_read, this,
                                    boost::asio::placeholders::error,
                                    boost::asio::placeholders::bytes_transferred));
        }
    } else {
        //error on read. Usually this means that the client disconnected, so
        //i destroy the client session object
        //std::cout << "error num:" << error.value() << ", error desc:" << error.message() << std::endl;
        delete this;
    }
} //void ClientSession::handle_read


void ClientSession::process_client_request()
{
    //as fast as possible i have to check if the requested file
    //exist in the cache. This way the latency is reduced to the minimum for
    //the files that exist in the cache.
    client_request_.cache_iterator = rocket::cache.cached_items.find(client_request_.hostname_and_uri);
    if (client_request_.cache_iterator != rocket::cache.cached_items.end()){
        //to arxeio yparxei stin cache
        send_file_from_cache();
    } else {
        //to arxeio den yparxei stin cache opote elegxw ean mporei na kataxwrithei afou prwta
        //elenksw ean o sindiasmos hostname kai uri yparxei
        //elegxw ean to hostname pou zitithike anikei se virtual server.
        //1. Ean yparxei, thetei sto client_request.response.absolute_hostname_and_requested_path_
        //to absolute path gia to uri pou zitithike.
        //2. Ean den yparxei tote psaxei ean yparxei default vistual host
        QFile file_io;
        if (ServerConfig::is_valid_requested_hostname(client_request_) == false){
            //TODO: den vrethike to hostname pou zitithike
            send_404_not_found_response();
            return;
        }

        //elegxw ean to path pou zitithike den einai malicious
        if(is_malicious_path(client_request_.response.absolute_hostname_and_requested_path)){
            send_400_bad_request_response();
            return;
        }

        if (try_get_request_uri_resource(file_io) == false){
            //the requested uri was not found.
            if (client_request_.response.server_config_map_it->second.allow_directory_listing == false){
                //if directory listing is not allowed (default) then send a 404 response
                send_404_not_found_response();
            } else {
                //if directory listing is allowed then send the directory listing
                if (try_send_directory_listing() == false){
                    //if the directory requested does not exist
                    //return 404
                    send_404_not_found_response();
                }
            }

            return;
        }

        //ok to arxeio pou zitithike yparxei opote tha to apothikefsw stin
        //cache, ean xwraei, kai sti synexeia tha to steilw ston client poy to zitise
        if (add_to_cache_if_fits(file_io)){
            //ok, to arxeio mpike stin cache opote to apostelw ston client
            send_file_from_cache();
            return;
        } else {
            //to arxeio den mporei na mpei stin cache, opote
            //to stelnw diavazontas to apeftheias apo to meso apothikefsis.
            client_request_.response.bytes_of_file_sent = 0;
            read_and_send_requested_file(file_io);
        }
    }

}

bool ClientSession::add_to_cache_if_fits(QFile &file_io){
    //elegxw ean to megethos tou arxeiou epitrepetai na mpei stin cache
    if (file_io.size() <= rocket::cache.max_file_size){
        //elegxw ean to neo arxeio epitrepetai na mpei stin cache
        auto file_size = file_io.size();
        auto cache_size_with_new_file = rocket::cache.cache_current_size + file_size;
        if (cache_size_with_new_file <= rocket::cache.cache_max_size){
            //ok mporei na mpei stin cache, opote to topothetw

            //fortwnw to axeio sto prosorino vector
            std::vector<char> response_body_vect(file_size);
            file_io.read(response_body_vect.data(), file_size);

            //enimerwnw to yparxon megethos tis cache
            rocket::cache.cache_current_size = cache_size_with_new_file;

            std::string mime_ = mime_db_.mimeTypeForFile(QString::fromStdString(client_request_.uri)).name().toStdString();

            QFileInfo qfile_info(file_io);
            std::string etag_ = rocket::get_next_etag();
            std::string modified_date_ = (qfile_info.lastModified().toString("ddd, dd MMM yyyy hh:mm:ss") + " GMT").toStdString(); //TODO: formating a QDateTime is very slow.
            CacheContent cache_content(response_body_vect,
                                       mime_,
                                       modified_date_,
                                       etag_
                                       );//TODO: na ylopoiisw diki mou function gia lipsi mime. einai ligo argi afti tou Qt


            //kataxwrw to file stin cache kai lamvanw referense pros afto
            //wste na ginei apostoli tou meta
            client_request_.cache_iterator = rocket::cache.cached_items.emplace(
                        std::make_pair(CacheKey(client_request_.hostname_and_uri,client_request_.response.absolute_hostname_and_requested_path), cache_content)).first;

            //TODO: afou topothetithike to arxeio stin cache, to kataxwrw kai sto filesystemwatcher
            rocket::cache.file_system_watcher->addPath(client_request_.response.absolute_hostname_and_requested_path);
            return true;
        } else {
            //cache does not have available space for the file to fit
            //so I cleanup the cache. Maybe in the next request the file will fit inside the cache
            rocket::cache.remove_older_items();
        }
    }
    return false; //to arxeio den epitrepetai na mpei stin cache
}


void ClientSession::send_file_from_cache(){

    if (client_request_.is_range_request == false){
        //apostelw prwta ta headers
        client_request_.response.header =
                HTTP_Response_Templates::_200_OK_UNTIL_DATE_VALUE_ +
                rocket::get_gmt_date_time(client_request_.cache_iterator->second.last_access_time) + //get the current datetime as a string and store the current datetime as a time_t value in the last_access_time field.
                HTTP_Response_Templates::_200_OK_UNTIL_CONTENT_TYPE_VALUE_ +
                client_request_.cache_iterator->second.mime_type +
                HTTP_Response_Templates::_200_OK_CONTENT_LENGTH_ +
                std::to_string(client_request_.cache_iterator->second.data.size()) +
                HTTP_Response_Templates::_200_OK_AFTER_CONTENT_LENGTH_VALUE_ +
                client_request_.cache_iterator->second.last_modified +
                HTTP_Response_Templates::_200_OK_AFTER_LAST_MODIFIED_ +
                client_request_.cache_iterator->second.etag +
                HTTP_Response_Templates::_200_OK_AFTER_ETAG_VALUE;

        std::vector<boost::asio::const_buffer> buffers;
        buffers.push_back(boost::asio::buffer(client_request_.response.header.data(), client_request_.response.header.size()));
        buffers.push_back(boost::asio::buffer(client_request_.cache_iterator->second.data.data(), client_request_.cache_iterator->second.data.size()));

        client_request_.response.current_state = ClientResponse::state::single_send;
        boost::asio::async_write(socket_,
                                 buffers,
                                 boost::bind(&ClientSession::handle_write, this,
                                 boost::asio::placeholders::error));
    } else {
        unsigned long long int range_size = (client_request_.range_until_byte - client_request_.range_from_byte) + 1;
        //apostelw prwta ta headers
        QString response_header = HTTP_Response_Templates::_206_PARTIAL_CONTENT_RESPONSE_HEADER.arg(
                    QString::fromStdString(client_request_.cache_iterator->second.mime_type),
                    QString::number(range_size),
                    QString::number(client_request_.range_from_byte),
                    QString::number(client_request_.range_until_byte),
                    QString::number(client_request_.cache_iterator->second.data.size())
                    );
        client_request_.response.header = response_header.toStdString();

        std::vector<boost::asio::const_buffer> buffers;
        buffers.push_back(boost::asio::buffer(client_request_.response.header.data(), client_request_.response.header.size()));
        buffers.push_back(boost::asio::buffer(client_request_.cache_iterator->second.data.data() + client_request_.range_from_byte, range_size));

        client_request_.response.current_state = ClientResponse::state::single_send;
        boost::asio::async_write(socket_,
                                 buffers,
                                 boost::bind(&ClientSession::handle_write, this,
                                 boost::asio::placeholders::error));
    }
}

//TOTO: na vrw taxytero tropo wste na mi xreiazetai metatropi apo to qstring _200_OK sto str::string response_header_str
void ClientSession::read_and_send_requested_file(QFile &file_io){
    size_t file_size = file_io.size();//to krataw edw mipws kai allaksei to size kai exei allo megethos to response vector

    //elegxw ean to arxeio tha apostalei me ti mia i se tmimata
    size_t remaining_file_size = file_size - client_request_.response.bytes_of_file_sent;
    size_t total_response_size = 0;
    //std::vector<char> response;
    client_request_.response.data.clear();
    if (remaining_file_size <= FILE_CHUNK_SIZE){
        if (client_request_.response.current_state == ClientResponse::state::begin){
            //lamvanw to mime tou arxeiou gia na to
            //steilw sto response
            QMimeType mime_type_ = mime_db_.mimeTypeForFile(QString::fromStdString(client_request_.uri));//TODO: einai grigori i function, alla kalitera na kanw diki mouylopoiisi gia mime types pou tha fortwnontai apo arxeio
            std::string response_header_str = HTTP_Response_Templates::_200_OK_NOT_CACHED_.arg(
                        mime_type_.name(),
                        QString::number(remaining_file_size)
                        ).toStdString();
            total_response_size =  file_size + response_header_str.size();
            client_request_.response.data.reserve(total_response_size);
            client_request_.response.data.insert(client_request_.response.data.end(), response_header_str.begin(), response_header_str.end());
            //apostoli mono enos mimatos
            client_request_.response.data.resize(total_response_size);
            file_io.read(client_request_.response.data.data() + response_header_str.size(), file_size);
        } else {
            //teleftaio tmima apo tin apostoli me polla tmimata
            total_response_size = remaining_file_size;
            client_request_.response.data.resize(total_response_size);
            file_io.seek(client_request_.response.bytes_of_file_sent);
            file_io.read(client_request_.response.data.data(), total_response_size);
        }

        client_request_.response.current_state = ClientResponse::state::single_send;
    } else {
        //apostoli se tmimata
        if (client_request_.response.current_state == ClientResponse::state::begin){
            //lamvanw to mime tou arxeiou gia na to
            //steilw sto response
            QMimeType mime_type_ = mime_db_.mimeTypeForFile(QString::fromStdString(client_request_.uri));//TODO: einai grigori i function, alla kalitera na kanw diki mouylopoiisi gia mime types pou tha fortwnontai apo arxeio
            std::string response_header_str = HTTP_Response_Templates::_200_OK_NOT_CACHED_.arg(
                        mime_type_.name(),
                        QString::number(remaining_file_size)
                        ).toStdString();

            total_response_size =  FILE_CHUNK_SIZE + response_header_str.size();
            client_request_.response.data.reserve(total_response_size);
            client_request_.response.data.insert(client_request_.response.data.end(), response_header_str.begin(), response_header_str.end());
            client_request_.response.data.resize(total_response_size);

            file_io.read(client_request_.response.data.data() + response_header_str.size(), FILE_CHUNK_SIZE);
        } else {
            //apostoli endiamesou tmimatos
            total_response_size =  FILE_CHUNK_SIZE;
            client_request_.response.data.resize(total_response_size);
            file_io.seek(client_request_.response.bytes_of_file_sent);
            file_io.read(client_request_.response.data.data(), FILE_CHUNK_SIZE);
        }
        client_request_.response.current_state = ClientResponse::state::chunk_send;
    }//if file_size chunck check

    boost::asio::async_write(socket_,
                             boost::asio::buffer(client_request_.response.data.data(), total_response_size),
                             boost::bind(&ClientSession::handle_write, this,
                             boost::asio::placeholders::error));
}

//prospatheia lipsis tou recource pou zitithike (arxeio / fakelos)
bool ClientSession::try_get_request_uri_resource(QFile &file_io){
    //prospathw na anoiksw to arxeio pou zitithike
    file_io.setFileName(client_request_.response.absolute_hostname_and_requested_path);
    if (file_io.open(QFileDevice::ReadOnly) == false) {
        //Ean den vrethike to arxeio pou zitithike, elegxw ta indexes
        if (ServerConfig::index_exists(client_request_, file_io) == false){
            //ean to arxeio den yparxei sto filesystem, epistrefw false, wste na ginei
            //elegxos sti synexeia, ean prokeitai gia directory, i na epistrafei 404 sfalma
            return false;
        }
    }
    return true;
}

bool ClientSession::is_malicious_path(QString &path)
{
    //Directory traversal attack
    //https://en.wikipedia.org/wiki/Directory_traversal_attack
    //https://www.owasp.org/index.php/Path_Traversal
    //https://www.owasp.org/index.php/File_System#Path_traversal
    //https://cwe.mitre.org/data/definitions/23.html

    return path.contains("..");
}

//TODO: should not use wstring. Should support utf8 filenames
bool ClientSession::try_send_directory_listing(){
    QDir directory;
    QFileInfoList list;
    std::wostringstream os;

    QString directory_ = client_request_.response.absolute_hostname_and_requested_path;
    if (directory.setCurrent(directory_) == false) return false;

    //directory
    directory.setFilter(QDir::NoDot | QDir::AllEntries);
    directory.setSorting(QDir::DirsFirst);
    list = directory.entryInfoList();
    std::wstring url_encoded;
    for (auto file:list){
        QString slash("/");
        if(file.isDir()){
            url_encoded = file.fileName().replace(" ", "%20").toStdWString() + slash.toStdWString();
            //einai directory
            os << "<br /><a href=""" << url_encoded << """>"
             << QString("<DIR> ").toHtmlEscaped().toStdWString()
            << file.fileName().toHtmlEscaped().toStdWString() << "</a>";
        }else {
            url_encoded = file.fileName().replace(" ", "%20").toStdWString();
            //einai arxeio
            os << "<br /><a href=""" << (QString::fromStdString(client_request_.uri).endsWith(slash) ?
                                             QString::fromStdString(client_request_.uri).toStdWString() :
                                             QString::fromStdString(client_request_.uri).toStdWString() +  slash.toStdWString())
                                        + url_encoded << """>"
            << file.fileName().toHtmlEscaped().toStdWString() << "</a>";
        }
    }//for loop

    std::wstring response_body = HTTP_Response_Templates::_DIRECTORY_LISTING_.arg(client_request_.response.absolute_hostname_and_requested_path,
                                                                     QString::fromStdWString(os.str())).toStdWString();

    //to length tou body se QString
    std::stringstream ssize_;
    ssize_ << response_body.size();
    std::string sService = ssize_.str();
    QString length_(QString::fromStdString(sService));

    QString mime_type("text/html");

    //topothetw ta header values sto response
    std::string resp_header_string = HTTP_Response_Templates::_200_OK_NOT_CACHED_.arg(mime_type, length_).toStdString();

    //prosthetw sto header to body

    std::vector<char> resp_vector(std::make_move_iterator(resp_header_string.begin()), std::make_move_iterator(resp_header_string.end()));
    resp_vector.insert(resp_vector.end(), std::make_move_iterator(response_body.begin()), std::make_move_iterator(response_body.end()));

    client_request_.response.current_state = ClientResponse::state::single_send;
    boost::asio::async_write(socket_,
                             boost::asio::buffer(resp_vector.data(), resp_vector.size()),
                             boost::bind(&ClientSession::handle_write, this,
                             boost::asio::placeholders::error));

    return true;
}

void ClientSession::send_404_not_found_response(){
    client_request_.response.header = QString(HTTP_Response_Templates::_404_NOT_FOUND_HEADER_.arg(
                       QString::number(HTTP_Response_Templates::_404_NOT_FOUND_BODY_.size())) %
                       HTTP_Response_Templates::_404_NOT_FOUND_BODY_).toStdString();

    client_request_.response.current_state = ClientResponse::state::single_send;
    boost::asio::async_write(socket_,
                             boost::asio::buffer(client_request_.response.header.data(), client_request_.response.header.size()),
                             boost::bind(&ClientSession::handle_write, this,
                             boost::asio::placeholders::error));
}

void ClientSession::send_400_bad_request_response(){
    client_request_.response.header = QString(HTTP_Response_Templates::_400_BAD_REQUEST_HEADER_.arg(
                       QString::number(HTTP_Response_Templates::_400_BAD_REQUEST_BODY_.size())) %
                       HTTP_Response_Templates::_400_BAD_REQUEST_BODY_).toStdString();

    client_request_.response.current_state = ClientResponse::state::single_send;
    boost::asio::async_write(socket_,
                             boost::asio::buffer(client_request_.response.header.data(), client_request_.response.header.size()),
                             boost::bind(&ClientSession::handle_write, this,
                             boost::asio::placeholders::error));
}

//
void ClientSession::handle_write(const boost::system::error_code& error)
{
    if (!error){
        if (client_request_.response.current_state == ClientResponse::state::single_send){
            client_request_.response.current_state = ClientResponse::state::begin;
            //i apostoli teleiwse xwris na xreiazetai na steilw kati allo, opote
            //kanw register to callback gia tin periptwsi poy tha yparxoun dedomena gia anagnwsi
            if (client_request_.connection == http_connection::keep_alive){
                socket_.async_read_some(boost::asio::buffer(client_request_parser_.data_.data(), REQUEST_BUFFER_SIZE),
                                        boost::bind(&ClientSession::handle_read, this,
                                        boost::asio::placeholders::error,
                                        boost::asio::placeholders::bytes_transferred));
            }else {
                //based on the headers of the client request we should close the connection
                socket().close();
            }

        } else if (client_request_.response.current_state == ClientResponse::state::chunk_send){
            //to arxeio prepei na apostalei se tmimata opote sinexizw tin apostoli apo ekei pou meiname
            client_request_.response.bytes_of_file_sent += FILE_CHUNK_SIZE;
            QFile file_io (client_request_.response.absolute_hostname_and_requested_path);
            file_io.open(QFileDevice::ReadOnly);
            read_and_send_requested_file(file_io);
        }

    } else {
        delete this;
    }
}//void ClientSession::handle_write

