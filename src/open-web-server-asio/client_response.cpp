#include "client_response.h"
#include <iostream>

ClientResponse::ClientResponse()
{
}

//rd = response data
//The Common Gateway Interface (CGI) Version 1.1
//https://tools.ietf.org/html/rfc3875#section-6.1.1
http_parser_result ClientResponse::parse(std::vector<char> &rd, ClientResponse &client_response)
{
    http_parser_result ret = http_parser_result::fail;
    http_parser_state parser_state = state_start;
    size_t parser_content_begin_index = 0;
    size_t bytes_transferred = rd.size();

    //because the Status: header
    size_t status_header_removal_begin,status_header_removal_end;

    //initial client response object clenup
    client_response.cgi__status = cgi_status::UNKNOWN;
    client_response.cgi_location.clear();

    for (size_t buffer_position = 0; buffer_position < bytes_transferred; buffer_position++){
        //if(buffer_position == 336){
        //    std::cout << "break here";
        //}

        switch (parser_state){

        // ******* CHECK EVERY HEADER ************
        case state_CRLF:
        case state_start:
            if (rd[buffer_position] == 'S'){
                parser_state = state_CGI_S;
                parser_content_begin_index = buffer_position;
                break;
            }

            if (rd[buffer_position] == 'L'){
                parser_state = state_CGI_L;
                parser_content_begin_index = buffer_position;
                break;
            }

            if (rd[buffer_position] == '\r'){
                if (parser_state == state_CRLF){
                    parser_state = state_CRLF_CR;
                }else{
                   parser_state = state_CR;
                }
                break;
            }

            parser_state = state_UNKNOWN_HEADER;
            break;
        // ******* CHECK EVERY HEADER ************

        case state_CGI_S:
            if (rd[buffer_position] == ':'){
                //Check for Status: header
                if (buffer_position - parser_content_begin_index == 6){
                    status_header_removal_begin = buffer_position - 6;
                    parser_state = state_CGI_STATUS_COLON;
                } else {
                    parser_state = state_UNKNOWN_HEADER;
                }
            }
            break;
        case state_CGI_STATUS_COLON:
            if (rd[buffer_position] != ' '){
                //Check status header value
                //6.3.3.  Status
                //Status         = "Status:" status-code SP reason-phrase NL
                //status-code    = "200" | "302" | "400" | "501" | extension-code
                //extension-code = 3digit
                //reason-phrase  = *TEXT
                if (rd[buffer_position] == '2'){
                    //Status: 200
                    client_response.cgi__status = cgi_status::_200_OK;
                    parser_state = state_CGI_STATUS_FOUND;
                }
                if (rd[buffer_position] == '3'){
                    //Status: 3xx
                    parser_state = state_CGI_STATUS_3XX;
                }
            }
            break;
        case state_CGI_STATUS_3XX:
            if (rd[buffer_position] == ' '){
                if (rd[buffer_position - 1] == '1'){
                    client_response.cgi__status = cgi_status::_301_MOVED_PERMANENTLY;
                    parser_state = state_CGI_STATUS_FOUND;
                }else if (rd[buffer_position - 1] == '2'){
                    client_response.cgi__status = cgi_status::_302_FOUND;
                    parser_state = state_CGI_STATUS_FOUND;
                }else if (rd[buffer_position - 1] == '3'){
                    client_response.cgi__status = cgi_status::_303_SEE_OTHER;
                    parser_state = state_CGI_STATUS_FOUND;
                } else {
                    parser_state = state_UNKNOWN_HEADER;
                }
            }
            break;
        case state_CGI_STATUS_FOUND:
            if (rd[buffer_position] == '\r'){
                if (buffer_position + 1 < bytes_transferred){
                    status_header_removal_end = buffer_position + 1;
                }
                parser_state = state_CR;
            }
            break;

        case state_CGI_L:
            if (rd[buffer_position] == ':'){
                //Check for Location: header
                if (buffer_position - parser_content_begin_index == 8){
                    parser_state = state_CGI_LOCATION_COLON;
                } else {
                    parser_state = state_UNKNOWN_HEADER;
                }
            }
            break;
        case state_CGI_LOCATION_COLON:
            if (rd[buffer_position] != ' '){
                parser_content_begin_index = buffer_position;
                parser_state = state_CGI_LOCATION_CONTENT;
            }
            break;
        case state_CGI_LOCATION_CONTENT:
            if (rd[buffer_position] == '\r'){
                parser_state = state_CR;
                client_response.cgi_location = std::string(rd.begin() + parser_content_begin_index,
                                     rd.begin() + parser_content_begin_index + (buffer_position - parser_content_begin_index));
            }
            break;

        case state_CR:
            if (rd[buffer_position] == '\n'){
                parser_state = state_CRLF;
            }
            break;
        case state_CRLF_CR:
            if (rd[buffer_position] == '\n'){
                //OK Parsing done
                //client_response.header_in = std::vector<char>(rd.begin(), rd.begin() + buffer_position + 1);
                if (buffer_position < bytes_transferred - 1){
                    //There are body data
                    client_response.body_in = std::vector<char>(rd.begin() + buffer_position + 1, rd.end());
                    //client_response.cgi_content_size =   //(rd.size() - buffer_position) - 1;
                } else {
                    client_response.body_in.clear();
                    //client_response.cgi_content_size = 0;
                    //client_response.body_in.clear();
                }
                parser_state = state_DONE;
                //maybe the best solution to exit from the switch and for loop.
                goto exit_loop;
            } else {
                //TODO: error: after state_CRLF_CR (\r\n\r) there should allways be a \n
            }
            break;

        case state_UNKNOWN_HEADER:
        default:
            if (rd[buffer_position] == '\r'){
                parser_state = state_CR;
            }
            break;


        }//switch
    }// for loop
    exit_loop: ;

    if (parser_state == state_DONE){
        ret = http_parser_result::success;
        if (client_response.cgi__status == cgi_status::UNKNOWN){
            client_response.cgi__status = cgi_status::_200_OK;
        } else {
            //a status header was found so I will remove it
            rd.erase(rd.begin() + status_header_removal_begin, rd.begin() + status_header_removal_end + 1);
        }
    }

    return ret;
}
