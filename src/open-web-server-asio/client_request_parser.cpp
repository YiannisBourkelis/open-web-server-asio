#include "client_request_parser.h"
#include <QStringBuilder>
#include <iostream>
#include <boost/asio.hpp>

using namespace enums;

ClientRequestParser::ClientRequestParser()
{
    data_.resize(REQUEST_BUFFER_SIZE);
}

//---- Possible parse algorithm improvements:
//- The first line of the request is allways GET/POST/HEAD etc. Checking for the request type can be done outside
//  of the main parser loop. The main parser loop should run for the headers with non-standard line position
//
//- I should not check for GET. I should only check for G and space in position 4. The same implies for
//  other comparisons. For example the HTTP/1.1 version can be extracted by searching only for / ,
//  5 positions after the first non-space character at the end of the URI.
//
//- Should check if the vector operator[] is prefered over the vector.at. The [] operator should be faster
//  but for some reason on my mac the .at method has slighter better spead (avg.460.000 req/sec vs 465.000 req/sec
//
//- Should benchmark an implementation with switch vs if statements
//
// ---- Things I tried and observed:
//- I tried std::make_move_iterator(data.begin()... to move the data to the object member string variables,
//  but I didn't measure any possitive difference in performance. Should try it again when there are more
//  headers to process and copy.
int ClientRequestParser::parse(std::vector<char> &data, size_t bytes_transferred, ClientRequest &client_request){

    http_parser_state current_state = start_state;

    for (size_t index_char = 0; index_char < bytes_transferred; index_char++){

        if (current_state == start_state)
        {
            if(data[index_char] == 71 && // G
                    ((index_char + 4) < bytes_transferred) &&
                    data[index_char + 1] == 69 && // E 69
                    data[index_char + 2] == 84 && // T
                    data[index_char + 3] == 32 // space
                    ) {
                index_char += 3;
                current_state = state_GET;//"G"
            }
            continue;
        }

        if (current_state == state_GET)
        {
            if(data[index_char] != 32) {// not space
               //start of GET/POST/HEAD URI
                client_request.parser_content_begin_index = index_char;
                current_state = state_GET_URI_CONTENT;
                continue;
            }
            continue;
        }

        if (current_state == state_GET_URI_CONTENT) {
            if (data[index_char] == 32) {// space after get uri
                client_request.uri = std::string(data.begin() + client_request.parser_content_begin_index,
                                                 data.begin() + index_char);
                current_state = state_GET_URI_CONTENT_END;
                continue;
            }
            continue;
        }

        //GET / HTTP/1.1
        if (current_state == state_GET_URI_CONTENT_END){
            // characters after the space after the uri means we have the http version
            if(data[index_char] == 72 && // H
                    ((index_char + 8) < bytes_transferred) &&
                    data[index_char + 1] == 84 && // T
                    data[index_char + 2] == 84 && // T
                    data[index_char + 3] == 80 && // P
                    data[index_char + 4] == 47 // /
                    ) {
                //check verdion number
                if (data[index_char + 5] == 49 && data[index_char + 7] == 49){
                    client_request.http_protocol_ver = http_protocol_version::HTTP_1_1;
                } else if (data[index_char + 5] == 49 && data[index_char + 7] == 48){
                    client_request.http_protocol_ver = http_protocol_version::HTTP_1_0;
                }
                index_char += 7;
                current_state = state_HTTP_VERSION;
            }
            continue;
        }

        if (current_state == state_CRLF)
        {
            //check for Host: header
            if (data[index_char] == 72 &&
                    ((index_char + 6) < bytes_transferred) &&
                    data[index_char + 1] == 111 &&
                    data[index_char + 2] == 115 && //s
                    data[index_char + 3] == 116 && //t
                    data[index_char + 4] == 58) { //:
                index_char += 5;
                current_state = state_HOST;
                continue;
            }


            //if (data[index_char] == 67 ){ // C
            //    std::cout << "hh";
            //}

            //check for Connection: keep-alive. Actually I only check for eep live
            if (data[index_char] == 67 && // C
                    ((index_char + 11) < bytes_transferred) &&
                    data[index_char + 1] == 111 && // o
                    data[index_char + 2] == 110 && // n
                    data[index_char + 3] == 110 && // n
                    data[index_char + 4] == 101 && //e
                    data[index_char + 5] == 99 && // c
                    data[index_char + 6] == 116 && // t
                    data[index_char + 7] == 105 && // i
                    data[index_char + 8] == 111 && // o
                    data[index_char + 9] == 110 && // n
                    data[index_char + 10]== 58 ) { // :
                index_char += 10;
                current_state = state_CONNECTION;
                continue;
            }

            //check if we are here because no known header that we are
            //inderested for exist.
            if (data[index_char] != '\r'){
                current_state = state_UNKNOWN_HEADER;
            } else if (data[index_char] == '\r' &&
                       ((index_char + 1) < bytes_transferred) &&
                       data[index_char + 1] == '\n'){
               index_char += 1;
               current_state = state_CRLF_x2;
            } else {
                std::cout << "parse error!";
                //TODO: the request is malformed. I should return an error
            }
            continue;
        }

        if (current_state == state_HOST)
        {
            if (data[index_char] != 32){ // space
                client_request.parser_content_begin_index = index_char;
                current_state = state_HOST_CONTENT;
            }
            continue;
        }

        if (current_state == state_HOST_CONTENT)
        {
            if (data[index_char] == 10){ // \n
                client_request.hostname = std::string(data.begin() + client_request.parser_content_begin_index,
                                                      data.begin() + index_char - 1);
                current_state = state_HOST_CONTENT_END;

            }
            continue;
        }

        if (current_state == state_CONNECTION){
            if (data[index_char] == 101) { // e. If it has an e it means it is keep-alive
                client_request.connection = http_connection::keep_alive;
                current_state = state_CONNECTION_KEEP_ALIVE_OR_CLOSE;
            } else if (data[index_char] == 111) { // o. If it has o it means it is close
                client_request.connection = http_connection::close;
                current_state = state_CONNECTION_KEEP_ALIVE_OR_CLOSE;
            }
            continue;
        }


        if (data[index_char] == '\r' &&
                ((index_char + 2) < bytes_transferred) &&
                data[index_char + 1] == '\n') {
                index_char += 1;
                current_state = state_CRLF;
            continue;
        }

    }//for loop


    //enonw to hostname me to uri
    client_request.hostname_and_uri = client_request.hostname + client_request.uri;
    client_request.is_range_request = false;

    //set the connection property to keep allive or close based on the request headers
    if (client_request.http_protocol_ver == http_protocol_version::HTTP_1_1){
        client_request.connection = http_connection::keep_alive;
    }else if (client_request.http_protocol_ver == http_protocol_version::HTTP_1_0 ||
              client_request.http_protocol_ver == http_protocol_version::HTTP_0_9){
        client_request.connection = http_connection::close;
    } else {
        client_request.connection = http_connection::unknown;
    }

    return 0;
}

int ClientRequestParser::parse2(std::vector<char> &data, size_t bytes_transferred, ClientRequest &client_request){

    http_parser_state current_state = start_state;

    for (size_t index_char = 0; index_char < bytes_transferred; index_char++){

        if (current_state == start_state)
        {
            if(data.at(index_char) == 71 && // G
                    ((index_char + 4) < bytes_transferred) &&
                    data.at(index_char + 1) == 69 && // E
                    data.at(index_char + 2) == 84 && // T
                    data.at(index_char + 3) == 32 // space
                    ) {
                index_char += 3;
                current_state = state_GET;//"G"
            }
            continue;
        }

        if (current_state == state_GET)
        {
            if(data.at(index_char) != 32) {// not space
               //start of GET/POST/HEAD URI
                client_request.parser_content_begin_index = index_char;
                current_state =state_GET_URI_CONTENT;
                continue;
            }
            continue;
        }

        if (current_state == state_GET_URI_CONTENT) {
            if (data.at(index_char) == 32) {// space after get uri
                //client_request.uri_ref = QStringRef(&client_request.raw_request, client_request.parser_content_begin_index, index_char - client_request.parser_content_begin_index);
                client_request.uri = std::string(data.begin() + client_request.parser_content_begin_index,
                                                 data.begin() + index_char);
                current_state = state_GET_URI_CONTENT_END;
                continue;
            }
            continue;
        }

        //GET / HTTP/1.1
        if (current_state == state_GET_URI_CONTENT_END){
            // characters after the space after the uri means we have the http version
            if(data.at(index_char) == 72 && // H
                    ((index_char + 8) < bytes_transferred) &&
                    data.at(index_char + 1) == 84 && // T
                    data.at(index_char + 2) == 84 && // T
                    data.at(index_char + 3) == 80 && // P
                    data.at(index_char + 4) == 47 // /
                    ) {
                //check verdion number
                if (data.at(index_char + 5) == 49 && data.at(index_char + 7) == 49){
                    client_request.http_protocol_ver = http_protocol_version::HTTP_1_1;
                } else if (data.at(index_char + 5) == 49 && data.at(index_char + 7) == 48){
                    client_request.http_protocol_ver = http_protocol_version::HTTP_1_0;
                }
                index_char += 7;
                current_state = state_HTTP_VERSION;
            }
            continue;
        }

        if (current_state == state_CRLF)
        {
            //check for Host: header
            if (data.at(index_char) == 72 &&
                    ((index_char + 6) < bytes_transferred) &&
                    data.at(index_char + 1) == 111 &&
                    data.at(index_char + 2) == 115 && //s
                    data.at(index_char + 3) == 116 && //t
                    data.at(index_char + 4) == 58) { //:
                index_char += 5;
                current_state = state_HOST;
                continue;
            }

            if (data.at(index_char) == 67 ){ // C
                std::cout << "hh";
            }

            //check for Connection: keep-alive. Actually I only check for eep live
            if (data.at(index_char) == 67 && // C
                    ((index_char + 11) < bytes_transferred) &&
                    data.at(index_char + 1) == 111 && // o
                    data.at(index_char + 2) == 110 && // n
                    data.at(index_char + 3) == 110 && // n
                    data.at(index_char + 4) == 101 && //e
                    data.at(index_char + 5) == 99 && // c
                    data.at(index_char + 6) == 116 && // t
                    data.at(index_char + 7) == 105 && // i
                    data.at(index_char + 8) == 111 && // o
                    data.at(index_char + 9) == 110 && // n
                    data.at(index_char + 10) == 58 ) { // :
                index_char += 10;
                current_state = state_CONNECTION;
                continue;
            }

            //check if we are here because no known header that we are
            //inderested for exist.
            if (data[index_char] != '\r'){
                current_state = state_UNKNOWN_HEADER;
            } else if (data[index_char] == '\r' &&
                       ((index_char + 1) < bytes_transferred) &&
                       data[index_char + 1] == '\n'){
               index_char += 1;
               current_state = state_CRLF_x2;
            } else {
                std::cout << "parse error!";
                //TODO: the request is malformed. I should return an error
            }
            continue;

        }

        if (current_state == state_HOST)
        {
            if (data.at(index_char) != 32){ // space
                client_request.parser_content_begin_index = index_char;
                current_state = state_HOST_CONTENT;
            }
            continue;
        }

        if (current_state == state_HOST_CONTENT)
        {
            if (data.at(index_char) == 10){ // \n
                client_request.hostname = std::string(data.begin() + client_request.parser_content_begin_index,
                                                      data.begin() + index_char - 1);
                current_state = state_HOST_CONTENT_END;

            }
            continue;
        }

        if (current_state == state_CONNECTION){
            if (data.at(index_char) == 101) { // e. If it has an e it means it is keep-alive
                client_request.connection = http_connection::keep_alive;
                current_state = state_CONNECTION_KEEP_ALIVE_OR_CLOSE;
            } else if (data.at(index_char) == 111) { // o. If it has o it means it is close
                client_request.connection = http_connection::close;
                current_state = state_CONNECTION_KEEP_ALIVE_OR_CLOSE;
            }
            continue;
        }

        if (data[index_char] == '\r' &&
                ((index_char + 2) < bytes_transferred) &&
                data[index_char + 1] == '\n') {
                index_char += 1;
                current_state = state_CRLF;
            continue;
        }

    }


    //enonw to hostname me to uri
    client_request.hostname_and_uri = client_request.hostname + client_request.uri;
    client_request.is_range_request = false;

    //set the connection property to keep allive or close based on the request headers
    if (client_request.http_protocol_ver == http_protocol_version::HTTP_1_1){
        client_request.connection = http_connection::keep_alive;
    }else if (client_request.http_protocol_ver == http_protocol_version::HTTP_1_0 ||
              client_request.http_protocol_ver == http_protocol_version::HTTP_0_9){
        client_request.connection = http_connection::close;
    } else {
        client_request.connection = http_connection::unknown;
    }

    return 0;
}

bool ClientRequestParser::proccess_new_data(size_t bytes_transferred, ClientRequest &client_request)
{
    //usually a client http request is less than 1024 bytes, so it fits
    //inside the buffer in a single read. Because of this and for performanse reasons
    //we should first check if it is a complete message (ends with \r\n\r\n)
    //The check for previous_request_data_.size() is done for the case where the previous
    //request was incomplete, so in this case we should proccess the request in the else statement that follows
    auto crlf_it = std::search(data_.begin(), data_.end(), crlf_crlf.begin(), crlf_crlf.end());
    auto first_index_of_crlfcrlf = std::distance(data_.begin(), crlf_it);
    if (first_index_of_crlfcrlf == bytes_transferred - 4 && previous_request_data_.size() == 0){
        parse(data_, bytes_transferred, client_request);
        return true;
    } else {
        //to request den exei symplirwthei akoma opote apothikevoume prosorina oti lifthike
        previous_request_data_.append(data_.data(), bytes_transferred);

        //efoson sto previous_request_data_ yparxei crlfcrlf tote
        //to mynima symplirwthike opote tha to lanw parse
        auto crlf_it = std::search(previous_request_data_.begin(), previous_request_data_.end(), crlf_crlf.begin(), crlf_crlf.end());
        auto first_index_of_crlfcrlf = std::distance(previous_request_data_.begin(), crlf_it);
        if (first_index_of_crlfcrlf > -1){
            parse(data_, bytes_transferred, client_request);
            return true;
            previous_request_data_.clear();
        } else {
            return false;
        }
    }

    return false;
}
