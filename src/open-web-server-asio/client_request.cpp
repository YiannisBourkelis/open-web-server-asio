#include "client_request.h"

#include <QStringBuilder>
#include <iostream>
#include <boost/asio.hpp>

using namespace enums;

ClientRequest::ClientRequest() : data(REQUEST_BUFFER_SIZE)
{
}

//https://en.wikipedia.org/wiki/List_of_HTTP_header_fields

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
http_parser_result ClientRequest::parse(ClientRequest &cr, size_t bytes_transferred){

    //std::string req__(data.begin(), data.begin()+bytes_transferred);
    //std::cout << req__ << "\r\n";

    for (size_t index_char = 0; index_char < bytes_transferred; index_char++){

        if (cr.parser_current_state == start_state)
        {
            if(cr.data[index_char] == 71 && // G
                    ((index_char + 4) < bytes_transferred) &&
                    cr.data[index_char + 1] == 69 && // E 69
                    cr.data[index_char + 2] == 84 && // T
                    cr.data[index_char + 3] == 32 // space
                    ) {
                index_char += 3;
                cr.parser_current_state = state_METHOD;
                cr.parser_current_state_index = index_char;
                cr.method = http_method::GET;
            } else if (cr.data[index_char] == 'P' &&
                       ((index_char + 5) < bytes_transferred) &&
                       cr.data[index_char + 1] == 'O' &&
                       cr.data[index_char + 2] == 'S' &&
                       cr.data[index_char + 3] == 'T' &&
                       cr.data[index_char + 4] == ' ' // space
                       ) {
                   index_char += 4;
                   cr.parser_current_state = state_METHOD;
                   cr.parser_current_state_index = index_char;
                   cr.method = http_method::POST;
            }
            continue;
        }

        if (cr.parser_current_state == state_METHOD)
        {
            if(cr.data[index_char] != 32) {// not space
               //start of GET/POST/HEAD URI
                cr.parser_content_begin_index = index_char;
                cr.parser_current_state = state_METHOD_URI_CONTENT;
                cr.parser_current_state_index = index_char;
                continue;
            }
            continue;
        }

        if (cr.parser_current_state == state_METHOD_URI_CONTENT) {
            if (cr.data[index_char] == 32) {// space after get uri
                cr.uri = std::string(cr.data.begin() + cr.parser_content_begin_index,
                                                 cr.data.begin() + index_char);
                cr.parser_current_state = state_METHOD_URI_CONTENT_END;
                cr.parser_current_state_index = index_char;
                continue;
            } else if (cr.data[index_char] == '?') {//begin of query string
                cr.uri = std::string(cr.data.begin() + cr.parser_content_begin_index,
                                                 cr.data.begin() + index_char);
                cr.parser_content_begin_index = index_char + 1;
                cr.parser_current_state = state_METHOD_URI_QUERYSTRING_CONTENT;
                cr.parser_current_state_index = index_char;
                continue;
            }
            continue;
        }

        if (cr.parser_current_state == state_METHOD_URI_QUERYSTRING_CONTENT) {
            if (cr.data[index_char] == ' ') {// space after get uri
                cr.query_string = std::string(cr.data.begin() + cr.parser_content_begin_index,
                                                 cr.data.begin() + index_char);
                cr.parser_current_state = state_METHOD_URI_CONTENT_END;
                cr.parser_current_state_index = index_char;
            }
            continue;
        }

        //GET / HTTP/1.1
        if (cr.parser_current_state == state_METHOD_URI_CONTENT_END){
            // characters after the space after the uri means we have the http version
            if(cr.data[index_char] == 72 && // H
                    ((index_char + 8) < bytes_transferred) &&
                    cr.data[index_char + 1] == 84 && // T
                    cr.data[index_char + 2] == 84 && // T
                    cr.data[index_char + 3] == 80 && // P
                    cr.data[index_char + 4] == 47 // /
                    ) {
                //check verdion number
                if (cr.data[index_char + 5] == 49 && cr.data[index_char + 7] == 49){
                    cr.http_protocol_ver = http_protocol_version::HTTP_1_1;
                } else if (cr.data[index_char + 5] == 49 && cr.data[index_char + 7] == 48){
                    cr.http_protocol_ver = http_protocol_version::HTTP_1_0;
                }
                index_char += 7;
                cr.parser_current_state = state_HTTP_VERSION;
                cr.parser_current_state_index = index_char;
            }
            continue;
        }

        if (cr.parser_current_state == state_CRLF)
        {
            //check for Host: header
            if (cr.data[index_char] == 72 &&
                    ((index_char + 6) < bytes_transferred) &&
                    cr.data[index_char + 1] == 111 &&
                    cr.data[index_char + 2] == 115 && //s
                    cr.data[index_char + 3] == 116 && //t
                    cr.data[index_char + 4] == 58) { //:
                index_char += 5;
                cr.parser_current_state = state_HOST;
                cr.parser_current_state_index = index_char;
                continue;
            }


            //if (data[index_char] == 67 ){ // C
            //    std::cout << "hh";
            //}

            //check for Connection: keep-alive. Actually I only check for keep live
            if (cr.data[index_char] == 67 && // C
                    ((index_char + 11) < bytes_transferred) &&
                    cr.data[index_char + 1] == 111 && // o
                    cr.data[index_char + 2] == 110 && // n
                    cr.data[index_char + 3] == 110 && // n
                    cr.data[index_char + 4] == 101 && //e
                    cr.data[index_char + 5] == 99 && // c
                    cr.data[index_char + 6] == 116 && // t
                    cr.data[index_char + 7] == 105 && // i
                    cr.data[index_char + 8] == 111 && // o
                    cr.data[index_char + 9] == 110 && // n
                    cr.data[index_char + 10]== 58 ) { // :
                index_char += 10;
                cr.parser_current_state = state_CONNECTION;
                cr.parser_current_state_index = index_char;
                continue;
            }

            //check if we are here because no known header that we are
            //inderested for exist.
            if (cr.data[index_char] != '\r'){
                cr.parser_current_state = state_UNKNOWN_HEADER;
            } else if (cr.data[index_char] == '\r' &&
                       ((index_char + 1) < bytes_transferred) &&
                       cr.data[index_char + 1] == '\n'){
               index_char += 1;
               cr.parser_current_state = state_CRLF_x2;
               cr.parser_current_state_index = index_char;
            } else {
                std::cout << "parse error!";
                //TODO: the request is malformed. I should return an error
            }
            continue;
        }

        if (cr.parser_current_state == state_HOST)
        {
            if (cr.data[index_char] != 32){ // space
                cr.parser_content_begin_index = index_char;
                cr.parser_current_state = state_HOST_CONTENT;
                cr.parser_current_state_index = index_char;
            }
            continue;
        }

        if (cr.parser_current_state == state_HOST_CONTENT)
        {
            if (cr.data[index_char] == 10){ // \n
                cr.hostname = std::string(cr.data.begin() + cr.parser_content_begin_index,
                                                      cr.data.begin() + index_char - 1);
                cr.parser_current_state = state_HOST_CONTENT_END;
                cr.parser_current_state_index = index_char;
            }
            continue;
        }

        if (cr.parser_current_state == state_CONNECTION){
            if (cr.data[index_char] == 101) { // e. If it has an e it means it is keep-alive
                cr.connection = http_connection::keep_alive;
                cr.parser_current_state = state_CONNECTION_KEEP_ALIVE_OR_CLOSE;
                cr.parser_current_state_index = index_char;
            } else if (cr.data[index_char] == 111) { // o. If it has o it means it is close
                cr.connection = http_connection::close;
                cr.parser_current_state = state_CONNECTION_KEEP_ALIVE_OR_CLOSE;
                cr.parser_current_state_index = index_char;
            }
            continue;
        }


        if (cr.data[index_char] == '\r' &&
                ((index_char + 2) < bytes_transferred) &&
                cr.data[index_char + 1] == '\n') {
                index_char += 1;
                cr.parser_current_state = state_CRLF;
                cr.parser_current_state_index = index_char;
            continue;
        }

    }//for loop


    //enonw to hostname me to uri
    cr.hostname_and_uri.clear();
    cr.hostname_and_uri.append(cr.hostname).append(cr.uri);
    //client_request.hostname_and_uri = client_request.hostname + client_request.uri;
    cr.is_range_request = false;

    //set the connection property to keep allive or close based on the request headers
    if (cr.connection == http_connection::unknown){
        if (cr.http_protocol_ver == http_protocol_version::HTTP_1_1){
            cr.connection = http_connection::keep_alive;
        }else if (cr.http_protocol_ver == http_protocol_version::HTTP_1_0 ||
                  cr.http_protocol_ver == http_protocol_version::HTTP_0_9){
            cr.connection = http_connection::close;
        }
    }

    return http_parser_result::success;
}
