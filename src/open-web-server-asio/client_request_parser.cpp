#include "client_request_parser.h"
#include <QStringBuilder>
#include <iostream>
#include <boost/asio.hpp>

using namespace enums;

ClientRequestParser::ClientRequestParser()
{
    data_.resize(REQUEST_BUFFER_SIZE);
}

/*
int ClientRequestParser::parse(QByteArray &data, ClientRequest &client_request){
    client_request.raw_request = QString(data);

    http_parser_state current_state = start_state;

    for (int index_char = 0; index_char < client_request.raw_request.size(); index_char++){
        switch (current_state) {
        case start_state:
        {
            if(client_request.raw_request[index_char] == "G" &&
                    client_request.raw_request[index_char + 1] == "E" &&
                    client_request.raw_request[index_char + 2] == "T"
                    ) {
                index_char += 2;
                current_state = state_GET;//"G"
            }
            break;
        }

        case state_GET:
        {
            if(client_request.raw_request[index_char] != " ") {// not space
               //start of GET/POST/HEAD URI
                client_request.parser_content_begin_index = index_char;
                current_state =state_GET_URI_CONTENT;
                break;
            }
            break;
        }

        case state_GET_URI_CONTENT:
        {
            if(client_request.raw_request[index_char] == " ") {// space after get uri
                client_request.uri_ref = QStringRef(&client_request.raw_request, client_request.parser_content_begin_index, index_char - client_request.parser_content_begin_index);
                client_request.uri = client_request.uri_ref.toString();
                current_state = state_GET_URI_CONTENT_END;
                break;
            }
            break;
        }

        case state_FIRST_CR:
        {
            //auto cc = client_request.raw_request.at(index_char);
            if (client_request.raw_request[index_char] == "\n") current_state = state_FIRST_LF;
            break;
        }

        case state_FIRST_LF:
        {
            if (client_request.raw_request[index_char] == "H" &&
                    ((index_char + 6) < client_request.raw_request.size()) &&
                    client_request.raw_request[index_char + 1] == "o" &&
                    client_request.raw_request[index_char + 2] == "s" &&
                    client_request.raw_request[index_char + 3] == "t" &&
                    client_request.raw_request[index_char + 4] == ":") {
                index_char += 5;
                current_state = state_HOST;
            }
            break;
        }

        case state_HOST:
        {
            if (client_request.raw_request[index_char] != " "){
                client_request.parser_content_begin_index = index_char;
                current_state = state_HOST_CONTENT;
            }
            break;
        }

        case state_HOST_CONTENT:
        {
            if (client_request.raw_request[index_char] == "\r"){
                client_request.host_ref = QStringRef(&client_request.raw_request, client_request.parser_content_begin_index, index_char - client_request.parser_content_begin_index);;
                client_request.hostname = client_request.host_ref.toString();
                current_state = state_HOST_CONTENT_END;
            }
            break;
        }

        default:
            //auto cc = client_request.raw_request[index_char];
            if (client_request.raw_request[index_char] == "\r") current_state = state_FIRST_CR;break;

        }//switch
    }


    //enonw to hostname me to uri
    client_request.hostname_and_uri = client_request.hostname % client_request.uri;
    client_request.is_range_request = false;

    return 0;
}
*/

int ClientRequestParser::parse(QByteArray &data, ClientRequest &client_request){
    client_request.raw_request = QString(data);

    http_parser_state current_state = start_state;

    int raw_size = client_request.raw_request.size();

    for (int index_char = 0; index_char < raw_size; index_char++){
        auto curr_char = client_request.raw_request.at(index_char);

        if (current_state == start_state)
        {
            if(curr_char == "G" &&
                    client_request.raw_request[index_char + 1] == "E" &&
                    client_request.raw_request[index_char + 2] == "T" &&
                    client_request.raw_request[index_char + 3] == " "
                    ) {
                index_char += 3;
                current_state = state_GET;//"G"
            }
            continue;
        }

        if (current_state == state_GET)
        {
            if(curr_char != " ") {// not space
               //start of GET/POST/HEAD URI
                client_request.parser_content_begin_index = index_char;
                current_state =state_GET_URI_CONTENT;
                continue;
            }
            continue;
        }

        if (current_state == state_GET_URI_CONTENT)
        {
            if (curr_char == " ") {// space after get uri
                client_request.uri_ref = QStringRef(&client_request.raw_request, client_request.parser_content_begin_index, index_char - client_request.parser_content_begin_index);
                client_request.uri = client_request.uri_ref.toString();
                current_state = state_GET_URI_CONTENT_END;
                continue;
            }
            continue;
        }

        if (current_state == state_FIRST_CR)
        {
            //auto cc = client_request.raw_request.at(index_char);
            if (curr_char == "\n") current_state = state_FIRST_LF;
            continue;
        }

        if (current_state == state_FIRST_LF)
        {
            if (curr_char == "H" &&
                    ((index_char + 6) < client_request.raw_request.size()) &&
                    client_request.raw_request[index_char + 1] == "o" &&
                    client_request.raw_request[index_char + 2] == "s" &&
                    client_request.raw_request[index_char + 3] == "t" &&
                    client_request.raw_request[index_char + 4] == ":") {
                index_char += 5;
                current_state = state_HOST;
            }
            continue;
        }

        if (current_state == state_HOST)
        {
            if (curr_char != " "){
                client_request.parser_content_begin_index = index_char;
                current_state = state_HOST_CONTENT;
            }
            continue;
        }

        if (current_state == state_HOST_CONTENT)
        {
            if (curr_char == "\r"){
                client_request.host_ref = QStringRef(&client_request.raw_request, client_request.parser_content_begin_index, index_char - client_request.parser_content_begin_index);;
                client_request.hostname = client_request.host_ref.toString();
            }
            continue;
        }

        if (curr_char == "\r") {
            current_state = state_FIRST_CR;
            continue;
        }

    }


    //enonw to hostname me to uri
    client_request.hostname_and_uri = client_request.hostname % client_request.uri;
    client_request.is_range_request = false;

    return 0;
}

//static
//TODO: implement the parser using a state machine
int ClientRequestParser::parse2(QByteArray &data, ClientRequest &client_request)
{
    //na koitaksw na to ylopoiisw me state machine.
    //isws to http://www.boost.org/doc/libs/1_66_0/libs/statechart/doc/index.html
    //na voithaei stin ylopoiisi

    //episis gia parsing na dw mipws einai kalo to boost.spirit

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

    //enonw to hostname me to uri
    client_request.hostname_and_uri = client_request.hostname % client_request.uri;

    //******************************************
    //elegxos ean einai HTTP/1.0
    //size_t http1_0 = request.find("HTTP/1.0", rv - 8);
    //if (http1_0 != std::string::npos){
    //    keep_alive = false;
    //}
    //******************************************


    //******************************************

    //lamvanw tyxwn byte range
     int range_pos = client_request.raw_request.indexOf("\nRange: bytes=",  http_method_line_end);
     client_request.is_range_request = (range_pos != -1);
     if (client_request.is_range_request){
         //vrethike range
         int range_dash = client_request.raw_request.indexOf("-", range_pos + 14);
         if (range_dash != -1){//vrethike i pavla px 0-1
            int range_end = client_request.raw_request.indexOf("\r", range_dash + 2);
            if (range_end != -1){
             QString from =  client_request.raw_request.mid(range_pos + 14,range_dash-(range_pos + 14));
             QString until = client_request.raw_request.mid(range_dash + 1, range_end - (range_dash + 1));
             client_request.range_from_byte = from.toULongLong();
             client_request.range_until_byte = until.toULongLong();
            }
         }
     }



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
