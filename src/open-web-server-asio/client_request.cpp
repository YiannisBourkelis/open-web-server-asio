#include "client_request.h"

#include <QStringBuilder>
#include <iostream>
#include <boost/asio.hpp>

using namespace enums;

ClientRequest::ClientRequest() : buffer(REQUEST_BUFFER_SIZE)
{
}

//https://en.wikipedia.org/wiki/List_of_HTTP_header_fields

//Building your own memory manager for C/C++ projects
//https://www.ibm.com/developerworks/aix/tutorials/au-memorymanager/index.html

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
    cr.total_bytes_transfered += bytes_transferred;

    //std::string req__(cr.buffer.begin(), cr.buffer.begin() + cr.total_bytes_transfered);
    //std::cout << req__ << "\r\n";

    for (; cr.buffer_position < cr.total_bytes_transfered; cr.buffer_position++){
        switch (cr.parser_state){
        // **** METHODS GET/POST *******
        // ----- G E T -----
        case state_start:
            if (cr.buffer[cr.buffer_position] == 'G'){
                cr.parser_state = state_GET_G;
            } else if (cr.buffer[cr.buffer_position] == 'P'){
                cr.parser_state = state_POST_P;
            }
            break;
        case state_GET_G:
            if (cr.buffer[cr.buffer_position] == 'E'){
                cr.parser_state = state_GET_E;
            }
            break;
        case state_GET_E:
            if (cr.buffer[cr.buffer_position] == 'T'){
                cr.parser_state = state_GET_T;
            }
            break;
        case state_GET_T:
            if (cr.buffer[cr.buffer_position] == ' '){
                cr.method = http_method::GET;
                cr.parser_state = state_SPACE_AFTER_METHOD;
            }
            break;
        // ----- G E T -----

        // ----- P O S T -----
        case state_POST_P:
            if (cr.buffer[cr.buffer_position] == 'O'){
                cr.parser_state = state_POST_O;
            }
            break;
        case state_POST_O:
            if (cr.buffer[cr.buffer_position] == 'S'){
                cr.parser_state = state_POST_S;
            }
            break;
        case state_POST_S:
            if (cr.buffer[cr.buffer_position] == 'T'){
                cr.parser_state = state_POST_T;
            }
            break;
        case state_POST_T:
            if (cr.buffer[cr.buffer_position] == ' '){
                cr.method = http_method::POST;
                cr.parser_state = state_SPACE_AFTER_METHOD;
            }
            break;
        // ----- P O S T -----
        //  METHODS GET/POST

        // **** URI/QUERYSTRING AFTER METHOD *******
        case state_SPACE_AFTER_METHOD:
            //First non space char after the request method marks the begining
            //of the URI
            if (cr.buffer[cr.buffer_position] != ' '){
                //begin of URI
                cr.parser_state = state_URI_CONTENT;
                cr.parser_content_begin_index = cr.buffer_position;
            }
            break;
        case state_URI_CONTENT:
            //Whe are at the URI characters until we find a space.
            if (cr.buffer[cr.buffer_position] == ' '){
                //begin of URI
                cr.parser_state = state_URI_CONTENT_END;
                cr.uri = std::string(cr.buffer.begin() + cr.parser_content_begin_index,
                                     cr.buffer.begin() + cr.parser_content_begin_index + (cr.buffer_position - cr.parser_content_begin_index));
            } else if (cr.buffer[cr.buffer_position] == '?'){
                //we found a ? inside the URI so we extract the URI path
                //and begin parsing the querystring part of the URI
                //but first we extract the URI path.
                cr.uri = std::string(cr.buffer.begin() + cr.parser_content_begin_index,
                                     cr.buffer.begin() + cr.parser_content_begin_index + (cr.buffer_position - cr.parser_content_begin_index));
                //mark the beginign of the querystring
                cr.parser_state = state_URI_QUERYSTRING_CONTENT;
                cr.parser_content_begin_index = cr.buffer_position;
            }
            break;
        case state_URI_CONTENT_END:
        case state_URI_QUERYSTRING_CONTENT_END:
            //after the end of uri is the HTTP version e.g. HTTP/1.1
            if (cr.buffer[cr.buffer_position] == 'H'){
                cr.parser_state = state_HTTP_H;
            }
            break;
        case state_URI_QUERYSTRING_CONTENT:
            //space after the querystring marks the end of the querystring
            if (cr.buffer[cr.buffer_position] == ' '){
                //extract the querystring
                cr.query_string = std::string(cr.buffer.begin() + cr.parser_content_begin_index + 1,
                                              cr.buffer.begin() + cr.parser_content_begin_index + (cr.buffer_position - cr.parser_content_begin_index));
                cr.parser_state = state_URI_QUERYSTRING_CONTENT_END;
            }
            break;


        // URI/QUERYSTRING AFTER METHOD

        // **** HTTP PROTOCOL VERSION *******
        case state_HTTP_H:
            //after the end of uri is the HTTP version e.g. HTTP/1.1
            if (cr.buffer[cr.buffer_position] == 'T'){
                cr.parser_state = state_HTTP_T;
            }
            break;
        case state_HTTP_T:
            //after the end of uri is the HTTP version e.g. HTTP/1.1
            if (cr.buffer[cr.buffer_position] == 'T'){
                cr.parser_state = state_HTTP_T_T;
            }
            break;
        case state_HTTP_T_T:
            //after the end of uri is the HTTP version e.g. HTTP/1.1
            if (cr.buffer[cr.buffer_position] == 'P'){
                cr.parser_state = state_HTTP_P;
            }
            break;
        case state_HTTP_P:
            //after the end of uri is the HTTP version e.g. HTTP/1.1
            if (cr.buffer[cr.buffer_position] == '/'){
                cr.parser_state = state_HTTP_SLASH;
            }
            break;
        case state_HTTP_SLASH:
            //after the end of uri is the HTTP version e.g. HTTP/1.1
            //if (cr.buffer[cr.buffer_position] >= 48 && cr.buffer[cr.buffer_position] <= 57){ //number between 0-9
                cr.parser_state = state_HTTP_VERSION_MAJOR_CONTENT;
                //cr.http_protocol_ver_major = cr.buffer[cr.buffer_position];
            //}
            break;
        case state_HTTP_VERSION_MAJOR_CONTENT:
            //after the end of uri is the HTTP version e.g. HTTP/1.1
            if (cr.buffer[cr.buffer_position] == '.'){
                cr.parser_state = state_HTTP_VERSION_DOT_CONTENT;
            }else {
                //TODO: error. there should be a . after the state_HTTP_VERSION_MAJOR_CONTENT
            }
            break;
        case state_HTTP_VERSION_DOT_CONTENT:
            //after the end of uri is the HTTP version e.g. HTTP/1.1
            //if (cr.buffer[cr.buffer_position] >= 48 && cr.buffer[cr.buffer_position] <= 57){ //number between 0-9
                cr.parser_state = state_HTTP_VERSION_MINOR_CONTENT;
                //cr.http_protocol_ver_minor = cr.buffer[cr.buffer_position];

                if (cr.buffer[cr.buffer_position - 2] == 49 && cr.buffer[cr.buffer_position] == 49){
                    cr.http_protocol_ver = http_protocol_version::HTTP_1_1;
                } else if (cr.buffer[cr.buffer_position - 2] == 49 && cr.buffer[cr.buffer_position] == 48){
                    cr.http_protocol_ver = http_protocol_version::HTTP_1_0;
                } else {
                    //TODO: error -> not supported HTTP version
                }
            //}
            break;
        // HTTP PROTOCOL VERSION

        // **** Host: HEADER - EXTRACT HOSTNAME *******
        case state_HOST_H:
            if (cr.buffer[cr.buffer_position] == 'o'){
                cr.parser_state = state_HOST_o;
            }
            break;
        case state_HOST_o:
            if (cr.buffer[cr.buffer_position] == 's'){
                cr.parser_state = state_HOST_s;
            }
            break;
        case state_HOST_s:
            if (cr.buffer[cr.buffer_position] == 't'){
                cr.parser_state = state_HOST_t;
            }
            break;
        case state_HOST_t:
            if (cr.buffer[cr.buffer_position] == ':'){
                cr.parser_state = state_HOST_COLON;
            }
            break;
        case state_HOST_COLON:
            if (cr.buffer[cr.buffer_position] != ' '){
                cr.parser_state = state_HOST_CONTENT;
                cr.parser_content_begin_index = cr.buffer_position;
            }
            break;
        case state_HOST_CONTENT:
            if (cr.buffer[cr.buffer_position] == '\r'){
                cr.parser_state = state_HOST_CONTENT_END;
                //cr.hostname = std::string(cr.buffer.begin() + cr.parser_content_begin_index,
                //                     cr.buffer.begin() + cr.parser_content_begin_index + (cr.buffer_position - cr.parser_content_begin_index));

                cr.hostname = std::string(cr.buffer.begin() + cr.parser_content_begin_index,
                                     cr.buffer.begin() + cr.parser_content_begin_index + (cr.buffer_position - cr.parser_content_begin_index));
                cr.hostname_and_uri.append(cr.hostname).append(cr.uri);
                cr.parser_state = state_CR;
            }
            break;
        // **** Conn: HEADER - EXTRACT HOSTNAME *******

        // **** HEADER starting with C ***********
        // **** [Content-Length], [Cookie], [Connection]
        case state_HEADER_C:
            if (cr.buffer[cr.buffer_position] == ':'){
               //check for Content-Length: header
               //e.g. Content-Length: 96\r\n
               size_t header_size = cr.buffer_position - cr.parser_content_begin_index;
               if (header_size == 14){
                   //I assume it is the Content-Length: header because it starts with
                   //C and has 14 letters size and no other header is like that.
                   cr.parser_state = state_CONTENT_LENGTH_COLON;
               } else if (header_size == 6){
                   //Assume Cookie Header
                   cr.parser_state = state_COOKIE_COLON;
               } else if (header_size == 10){
                   //Assume Connection header
                   cr.parser_state = state_CONNECTION_COLON;
               } else {
                   cr.parser_state = state_UNKNOWN_HEADER;
               }
            }
            break;
        // **** HEADER starting with C ***********

        // **** HEADER starting with U ***********
        // **** [User-Agent], [Upgrade]
        case state_HEADER_U:
            if (cr.buffer[cr.buffer_position] == ':'){
               size_t header_size = cr.buffer_position - cr.parser_content_begin_index;

               //check for User-Agent: header
               //e.g. User-Agent: Mozilla/5.0 (X11; Linux x86_64; rv:12.0) Gecko/20100101 Firefox/12.0\r\n
               if (header_size == 10){
                   //I assume it is the User-Agent: header because it starts with
                   //U and has 10 letters size and no other header is like that.
                   cr.parser_state = state_USER_AGENT_COLON;
               //} else if (header_size == 10){
               //    //Assume Connection header
               //    cr.parser_state = state_CONNECTION_COLON;
               } else {
                   cr.parser_state = state_UNKNOWN_HEADER;
               }
            }
            break;
        // **** HEADER starting with U ***********

        // ********* Content-Length: *************
        case state_CONTENT_LENGTH_COLON:
            if (cr.buffer[cr.buffer_position] != ' '){
                cr.parser_content_begin_index = cr.buffer_position;
                cr.parser_state = state_CONTENT_LENGTH_CONTENT;
            }
            break;
        case state_CONTENT_LENGTH_CONTENT:
            if (cr.buffer[cr.buffer_position] == '\r'){
                cr.parser_state = state_CR;
                bool is_valid_content_length_value = true;
                if (cr.buffer_position - cr.parser_content_begin_index > 13){
                    //TODO: error if content-length value is too big.
                    //limit the maximum content-size value to a maximum of
                    //a 13-digit number - around 9TB
                }
                for (size_t i = cr.parser_content_begin_index; i < cr.buffer_position; i++){
                    if (cr.buffer[i] < 48 || cr.buffer[i] > 57){
                        is_valid_content_length_value = false;
                        //TODO: error: the Content-Length value should be a number
                    }
                }//for i
                //ok content length value is a number, lets store it
                std::string content_length_string(std::string(cr.buffer.begin() + cr.parser_content_begin_index,
                               cr.buffer.begin() + cr.parser_content_begin_index + (cr.buffer_position - cr.parser_content_begin_index)));
                cr.content_length = std::stoull(content_length_string);
            }
            break;
         // *****************************************

         // ********* Cookie: **********************
        case state_COOKIE_COLON:
            if (cr.buffer[cr.buffer_position] != ' '){
                cr.parser_content_begin_index = cr.buffer_position;
                cr.parser_state = state_COOKIE_CONTENT;
            }
            break;
        case state_COOKIE_CONTENT:
            if (cr.buffer[cr.buffer_position] == '\r'){
                cr.parser_state = state_CR;
                cr.cookie = std::string(cr.buffer.begin() + cr.parser_content_begin_index,
                                     cr.buffer.begin() + cr.parser_content_begin_index + (cr.buffer_position - cr.parser_content_begin_index));
            }
            break;
         // ****************************************

        // ********* Connection: **********************
       case state_CONNECTION_COLON:
           if (cr.buffer[cr.buffer_position] != ' '){
               cr.parser_content_begin_index = cr.buffer_position;
               cr.parser_state = state_CONNECTION_CONTENT;
           }
           break;
       case state_CONNECTION_CONTENT:
           if (cr.buffer[cr.buffer_position] == '\r'){
               cr.parser_state = state_CR;
               // Because the keep alive or close might begin with an upper case or lower case
               // I am comparing only the second letter. k[e]ep-alive or c[l]ose
               if(cr.buffer[cr.buffer_position] == 'e'){
                   cr.connection = http_connection::keep_alive;
               } else if (cr.buffer[cr.buffer_position] == 'o'){
                   cr.connection = http_connection::close;
               } else {
                   cr.connection = http_connection::unknown;
                   //TODO: shoould check what to do if connection if upgrade
                   //And if it is not, if should I raise a parse error
                   //or simply try to figure out the most appropriate connection type
                   //e.g.: keep-alive for HTTP/1.1
               }
           }
           break;
        // ****************************************

       // ********* User-Agent: **********************
      case state_USER_AGENT_COLON:
          if (cr.buffer[cr.buffer_position] != ' '){
              cr.parser_content_begin_index = cr.buffer_position;
              cr.parser_state = state_USER_AGENT_CONTENT;
          }
          break;
      case state_USER_AGENT_CONTENT:
          if (cr.buffer[cr.buffer_position] == '\r'){
              cr.parser_state = state_CR;
              cr.user_agent = std::string(cr.buffer.begin() + cr.parser_content_begin_index,
                                   cr.buffer.begin() + cr.parser_content_begin_index + (cr.buffer_position - cr.parser_content_begin_index));
          }
          break;
       // ****************************************


        // \\\\**** BEGIN OF EVERY HEADER *******////
        case state_CRLF:
            // ****** ALL HEADERS COME AFTER A state_CRLF ********
            if (cr.buffer[cr.buffer_position] == 'H'){
                cr.parser_state = state_HOST_H;
                break;
            }

            //Content-Length: 96 or
            if (cr.buffer[cr.buffer_position] == 'C'){
                cr.parser_state = state_HEADER_C;
                cr.parser_content_begin_index = cr.buffer_position;
                break;
            }

            //User-Agent: or Upgrade
            if (cr.buffer[cr.buffer_position] == 'U'){
                cr.parser_state = state_HEADER_U;
                cr.parser_content_begin_index = cr.buffer_position;
                break;
            }



            if (cr.buffer[cr.buffer_position] == '\r'){
                cr.parser_state = state_CRLF_CR;
                break;
            }

            cr.parser_state = state_UNKNOWN_HEADER;
            break;
        // \\\\**** BEGIN OF EVERY HEADER *******////

        case state_QUERYSTRING_IN_BODY_CONTENT_BEGIN:
            cr.request_body.clear();
            cr.request_body.append(1, cr.buffer[cr.buffer_position]);
            cr.content_length--;
            cr.parser_state = state_QUERYSTRING_IN_BODY_CONTENT;
            break;
        case state_QUERYSTRING_IN_BODY_CONTENT:
            cr.content_length--;
            cr.request_body.append(1, cr.buffer[cr.buffer_position]);
            if (cr.content_length == 0){
                cr.parser_state = state_DONE;
            }
            break;


        case state_CR:
            if (cr.buffer[cr.buffer_position] == '\n'){
                cr.parser_state = state_CRLF;
            }
            break;
        case state_CRLF_CR:
            if (cr.buffer[cr.buffer_position] == '\n'){
                cr.parser_state = state_CRLF_CRLF;

                if (cr.content_length > 0){
                    //we are at the first \r\n\r\n and we have a content-length > 0
                    //so we will start copying the request body to the query_string variable
                    cr.parser_state = state_QUERYSTRING_IN_BODY_CONTENT_BEGIN;
                } else {
                    cr.parser_state = state_DONE;
                }
                //return http_parser_result::success;
            } else {
                //TODO: error: after state_CRLF_CR (\r\n\r) there should allways be a \n
            }
            break;

        case state_UNKNOWN_HEADER:
        default:
            if (cr.buffer[cr.buffer_position] == '\r'){
                cr.parser_state = state_CR;
            }
            break;

        }//switch
    } // parser for loop


    if (cr.parser_state != state_DONE){
        if (cr.buffer.size() == cr.total_bytes_transfered){
            cr.buffer.resize(cr.buffer.size() + cr.REQUEST_BUFFER_SIZE);
        }
        return http_parser_result::incomplete;
    } else if (cr.parser_state == state_DONE){
        //parsing DONE!
        //determine the connection type (keep alive or close) if there was no Connection header found
        if (cr.connection == http_connection::unknown){
            if (cr.http_protocol_ver == http_protocol_version::HTTP_1_1){
                cr.connection = http_connection::keep_alive;
            }else if (cr.http_protocol_ver == http_protocol_version::HTTP_1_0 ||
                      cr.http_protocol_ver == http_protocol_version::HTTP_0_9){
                cr.connection = http_connection::close;
            }
        }
        //return success so that we can begin proccessing the parsed request
        return http_parser_result::success;
    }else {
        //std::cout << "ok";
    }

    return http_parser_result::fail;
}

void ClientRequest::cleanup()
{
    if (parser_state == s::state_DONE){
        parser_state = state_start;
        //content_size = 0;
        connection = http_connection::unknown;
        hostname_and_uri.clear();
        cookie.clear();
        user_agent.clear();
        //parser_current_state_index = 0;
        //parser_previous_state_index = 0;
        buffer_position = 0;
        total_bytes_transfered = 0;
        is_range_request = false;
    }
}


/*
http_parser_result ClientRequest::parse(ClientRequest &cr, size_t bytes_transferred){


    size_t index_char = cr.parser_current_state_index;
    size_t total_bytes_transferred = bytes_transferred + cr.parser_previous_state_index;

    std::string req__(cr.data.begin(), cr.data.begin()+total_bytes_transferred);
    std::cout << req__ << "\r\n";

    for (; index_char < total_bytes_transferred; index_char++){

        if (cr.parser_current_state == start_state)
        {
            if(cr.data[index_char] == 71 && // G
                    ((index_char + 4) < total_bytes_transferred) &&
                    cr.data[index_char + 1] == 69 && // E 69
                    cr.data[index_char + 2] == 84 && // T
                    cr.data[index_char + 3] == 32 // space
                    ) {
                index_char += 3;
                cr.parser_current_state = state_METHOD;
                cr.parser_current_state_index = index_char;
                cr.method = http_method::GET;
            } else if (cr.data[index_char] == 'P' &&
                       ((index_char + 5) < total_bytes_transferred) &&
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
                    ((index_char + 8) < total_bytes_transferred) &&
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
                    ((index_char + 6) < total_bytes_transferred) &&
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
                    ((index_char + 11) < total_bytes_transferred) &&
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
                cr.parser_current_state_index = index_char + 1;
                continue;
            }

            //check if we are here because no known header that we are
            //inderested for exist.
            if (cr.data[index_char] != '\r'){
                cr.parser_last_known_state = cr.parser_current_state;
                cr.parser_current_state = state_UNKNOWN_HEADER;
            } else if (cr.data[index_char] == '\r' &&
                       ((index_char + 1) < total_bytes_transferred) &&
                       cr.data[index_char + 1] == '\n'){

               // ****** important point *********
               // we found the \r\n\r\n
               if (cr.content_size == 0){
                   // end of http request
                   // if we proccessed all bytes then
                   // we are done
                   if (index_char == total_bytes_transferred - 2){
                       cr.parser_current_state = state_DONE;
                       break;
                   }
               }
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
            if (cr.data[index_char] == '\r'){ //
                cr.hostname = std::string(cr.data.begin() + cr.parser_content_begin_index,
                                                      cr.data.begin() + index_char - 1);
                cr.parser_current_state = state_HOST_CONTENT_END;
                cr.parser_current_state_index = index_char;
            }
            //continue;
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
                ((index_char + 2) < total_bytes_transferred) &&
                cr.data[index_char + 1] == '\n') {
                index_char += 1;
                cr.parser_current_state = state_CRLF;
                cr.parser_current_state_index = index_char + 1;
            continue;
        }

    }//for loop

    if (cr.parser_current_state != state_DONE){
        if (cr.data.size() == index_char){
            //the buffer is full but the http request is not complete
            //so we should recieve the rest of it. We should keep
            //the data on the existing buffer because we might need
            //to extract some values from it while parsing, so we resize
            //the buffer, to give it more space for the resto of the
            //http request
            cr.data.resize(cr.data.size() + cr.REQUEST_BUFFER_SIZE);
            //cr.buffer_position = cr.data.size() - 1;
            cr.parser_previous_state_index = index_char;
            if (cr.parser_current_state == http_parser_state::state_UNKNOWN_HEADER){
                cr.parser_current_state = cr.parser_last_known_state;
            }
            return http_parser_result::incomplete;
        }
    }


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
*/

