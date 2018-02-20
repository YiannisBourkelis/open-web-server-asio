#include "client_request_parser.h"
#include <QStringRef>

ClientRequestParser::ClientRequestParser()
{
    data_.resize(REQUEST_BUFFER_SIZE);
}

//static
int ClientRequestParser::parse(QByteArray &data, ClientRequest &client_request)
{
    //na koitaksw na to ylopoiisw me state machine.
    //isws to http://www.boost.org/doc/libs/1_66_0/libs/statechart/doc/index.html
    //na voithaei stin ylopoiisi

    //metatrepw to client request se string gia na to analysw
  client_request.raw_request = QString(data);


    //******************************************
    //lamvanw to directory pou zitithike
    //GET dir /r/n
    //int get_start = request.find("G");
    int http_method_line_end = 0;
    bool is_get_request = data.startsWith("G");
    if (is_get_request){
        int get_end = data.indexOf(" ", 4);
        client_request.uri = client_request.raw_request.mid(4, get_end - 4);
        http_method_line_end = get_end + 9;
    }
    //std::string url(request.begin() + 4, request.begin() + get_end);
    //int http_method_line_end = get_end + 9;
    //request_uri = std::move(QString::fromStdString(url).replace("%20", " "));

    //request_uri = "/../../../";
    //request_uri = "/../../../back_f2.png";
    //request_uri = "././.";

    //url = "" + url + "";
    //******************************************

    //******************************************
    // lamvanw to hostname
    int hostname_idx = client_request.raw_request.indexOf("\nHost: ", http_method_line_end + 1);
    if (hostname_idx != -1){
        //host name beginning found
        int hostname_idx_end = client_request.raw_request.indexOf("\r", hostname_idx + 7);
        if (hostname_idx_end != -1){
            //host name end found
            client_request.hostname = client_request.raw_request.mid(hostname_idx + 7, hostname_idx_end - (hostname_idx + 7));
            //std::string hostname_str(request.begin() + hostname_idx + 6, request.begin() + hostname_idx_end);
        }
    }
    //******************************************

    //******************************************
    //elegxos ean einai HTTP/1.0
    //size_t http1_0 = request.find("HTTP/1.0", rv - 8);
    //if (http1_0 != std::string::npos){
    //    keep_alive = false;
    //}
    //******************************************


    //******************************************
    /*
    //lamvanw tyxwn byte range
     size_t range_pos = request.find("\nRange: bytes=", get_end_idx + 2);
     if (range_pos != std::string::npos){
         //vrethike range
         size_t range_dash = request.find("-", range_pos + 14);
         if (range_dash != std::string::npos){//vrethike i pavla px 0-1
            size_t range_end = request.find("\r", range_dash + 2);
            if (range_end != std::string::npos){
             std::string from(request.begin() + range_pos + 14,
                              request.begin() + range_dash);

             std::string until(request.begin() + range_dash + 1,
                               request.begin() + range_end);
             range_from_byte = std::stoull(from);
             range_until_byte = std::stoull(until);
             //char *end;
             //range_until_byte = std::strtoull(until.c_str(), &end, 10);
             has_range_header = true;

             //std::cout << "ok";
            }
         }
     }
     */


    //******************************************

     //******************************************
     //elegxw yparksi connection: close
     //keep_alive = (request.find("\nConnection: close", get_end_idx) == std::string::npos);

     //******************************************
    //is_dirty = false;

    //request_path = "/usr/local/var/www/72b.html";
    return 0;
}

bool ClientRequestParser::proccess_new_data(size_t bytes_transferred, ClientRequest &client_request)
{
    //usually a client http request is less than 1024 bytes, so it fits
    //inside the buffer in a single read. Because of this and for performanse reasons
    //we should first check if it is a complete messege (ends with \r\n\r\n)
    //The check for previous_request_data_.size() is done for the case where the previous
    //request was incomplete, so in this case we should proccess the request in the else statement that follows
    int first_index_of_crlfcrlf = data_.indexOf(crlf_crlf);
    if (first_index_of_crlfcrlf == bytes_transferred - 4 && previous_request_data_.size() == 0){
        parse(data_, client_request);
        return true;
    } else {
        //to request den exei symplirwthei akoma opote apothikevoume prosorina oti lifthike
        previous_request_data_.append(data_.data(), bytes_transferred);

        //efoson sto previous_request_data_ yparxei crlfcrlf tote
        //to mynima symplirwthike opote tha to lanw parse
        first_index_of_crlfcrlf = previous_request_data_.indexOf(crlf_crlf);
        if (first_index_of_crlfcrlf > -1){
            parse(data_, client_request);
            return true;
            previous_request_data_.clear();
        } else {
            return false;
        }
    }

    return false;
}
