#include "client_session.h"

//constructor
ClientSession::ClientSession(boost::asio::io_service& io_service) : socket_(io_service)
{
    //resize the buffer to accept the request.
    data_.resize(REQUEST_BUFFER_SIZE);
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
    socket_.async_read_some(boost::asio::buffer(data_.data(), REQUEST_BUFFER_SIZE),
                            boost::bind(&ClientSession::handle_read, this,
                            boost::asio::placeholders::error,
                            boost::asio::placeholders::bytes_transferred));
}

void ClientSession::handle_read(const boost::system::error_code& error, size_t bytes_transferred)
{
    if (!error)
    {
        //usually a client http request is less than 1024 bytes, so it fits
        //inside the buffer in a single read. Because of this and for performanse reasons
        //we should first check if it is a complete messege (ends with \r\n\r\n)
        //The check for previous_request_data_.size() is done for the case where the previous
        //request was incomplete, so in this case we should proccess the request in the else statement that follows
        int first_index_of_crlfcrlf = data_.indexOf(crlf_crlf);
        if (first_index_of_crlfcrlf == bytes_transferred - 4 && previous_request_data_.size() == 0){
        std::string respstr ("HTTP/1.1 200 OK\r\n"
                                   "Content-Type: text/html; charset=utf-8\r\n"
                                   "Content-Length: 5"
                                   "\r\n\r\n"
                                   "Hello");


        boost::asio::async_write(socket_,
                                 boost::asio::buffer(respstr.data(), respstr.size()),
                                 boost::bind(&ClientSession::handle_write, this,
                                 boost::asio::placeholders::error));

        } else {
            //to request den exei symplirwthei akoma opote apothikevoume prosorina oti lifthike
            previous_request_data_.append(data_.data(), bytes_transferred);

            //efoson sto previous_request_data_ yparxei crlfcrlf tote
            //to mynima symplirwthike opote tha to lanw parse
            first_index_of_crlfcrlf = previous_request_data_.indexOf(crlf_crlf);
            if (first_index_of_crlfcrlf > -1){
                std::string respstr ("HTTP/1.1 200 OK\r\n"
                                           "Content-Type: text/html; charset=utf-8\r\n"
                                           "Content-Length: 5"
                                           "\r\n\r\n"
                                           "Hello");
                boost::asio::async_write(socket_,
                                         boost::asio::buffer(respstr.data(), respstr.size()),
                                         boost::bind(&ClientSession::handle_write, this,
                                         boost::asio::placeholders::error));
                previous_request_data_.clear();
            } else {
                socket_.async_read_some(boost::asio::buffer(data_.data(), REQUEST_BUFFER_SIZE),
                                        boost::bind(&ClientSession::handle_read, this,
                                        boost::asio::placeholders::error,
                                        boost::asio::placeholders::bytes_transferred));
            }
        }


        //Read some data from the client and invoke the callback
        //to proccess the client request
        /*
        socket_.async_read_some(boost::asio::buffer(data_.data(), REQUEST_BUFFER_SIZE),
                                boost::bind(&ClientSession::handle_read, this,
                                boost::asio::placeholders::error,
                                boost::asio::placeholders::bytes_transferred));
                                */


    } else {
        //error on read. Usually this means that the client disconnected, so
        //i destroy the client session object
        //std::cout << "error num:" << error.value() << ", error desc:" << error.message() << std::endl;
        delete this;
    }
} //void ClientSession::handle_read


//
void ClientSession::handle_write(const boost::system::error_code& error)
{
    if (!error){

        socket_.async_read_some(boost::asio::buffer(data_.data(), REQUEST_BUFFER_SIZE),
                                boost::bind(&ClientSession::handle_read, this,
                                boost::asio::placeholders::error,
                                boost::asio::placeholders::bytes_transferred));

    } else {
        delete this;
    }
}//void ClientSession::handle_write
