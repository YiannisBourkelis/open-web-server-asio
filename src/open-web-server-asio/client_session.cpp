#include "client_session.h"
#include "server_config.h"
#include "http_response_templates.h"

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
    //prospatheia lipsis tou recource pou zitithike (arxeio / fakelos)
    QFile file_io;
    if (try_get_request_uri_resource(file_io)){
        // vrethike to arxeio, opote to diavazw gia na to stelnw ston client pou to zitise
        client_request_.response.bytes_of_file_sent = 0;
        read_requested_file(file_io);
    }
}

//TOTO: na vrw taxytero tropo wste na mi xreiazetai metatropi apo to qstring _200_OK sto str::string response_header_str
void ClientSession::read_requested_file(QFile &file_io){
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
            QMimeType mime_type_ = mime_db_.mimeTypeForFile(client_request_.uri);//TODO: einai grigori i function, alla kalitera na kanw diki mouylopoiisi gia mime types pou tha fortwnontai apo arxeio
            std::string response_header_str = HTTP_Response_Templates::_200_OK.arg(mime_type_.name(), QString::number(remaining_file_size)).toStdString();
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
            QMimeType mime_type_ = mime_db_.mimeTypeForFile(client_request_.uri);//TODO: einai grigori i function, alla kalitera na kanw diki mouylopoiisi gia mime types pou tha fortwnontai apo arxeio
            std::string response_header_str = HTTP_Response_Templates::_200_OK.arg(mime_type_.name(), QString::number(remaining_file_size)).toStdString();

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
    //elegxw ean to hostname pou zitithike anikei se virtual server.
    //1. Ean anikei, thetei sto client_request.response.absolute_hostname_and_requested_path_
    //to absolute path gia to uri pou zitithike.
    //2. Ean den anikei tote psaxei ean yparxei default vistual host
    if (ServerConfig::is_valid_requested_hostname(client_request_) == false){
        return false;
    }

    //elegxw ean to path pou zitithike den einai malicious
    if(is_malicious_path(client_request_.response.absolute_hostname_and_requested_path)) return false;

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

//
void ClientSession::handle_write(const boost::system::error_code& error)
{
    if (!error){

        if (client_request_.response.current_state == ClientResponse::state::single_send){
            client_request_.response.current_state = ClientResponse::state::begin;
            //i apostoli teleiwse xwris na xreiazetai na steilw kati allo, opote
            //kanw register to callback gia tin periptwsi poy tha yparxoun dedomena gia anagnwsi
            socket_.async_read_some(boost::asio::buffer(client_request_parser_.data_.data(), REQUEST_BUFFER_SIZE),
                                    boost::bind(&ClientSession::handle_read, this,
                                    boost::asio::placeholders::error,
                                    boost::asio::placeholders::bytes_transferred));
        } else if (client_request_.response.current_state == ClientResponse::state::chunk_send){
            //to arxeio prepei na apostalei se tmimata opote sinexizw tin apostoli apo ekei pou meiname
            client_request_.response.bytes_of_file_sent += FILE_CHUNK_SIZE;
            QFile file_io (client_request_.response.absolute_hostname_and_requested_path);
            file_io.open(QFileDevice::ReadOnly);
            read_requested_file(file_io);
        }

    } else {
        delete this;
    }
}//void ClientSession::handle_write

