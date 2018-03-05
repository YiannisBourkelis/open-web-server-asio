#include "client_response.h"
#include <iostream>

ClientResponse::ClientResponse()
{
}

int ClientResponse::parse(std::vector<char> &response_data, size_t bytes_transferred, ClientResponse &client_response)
{
    http_parser_state current_state = s::state_start;

    for (size_t index_char = 0; index_char < bytes_transferred; index_char++){
        if (current_state == s::state_CRLF)
        {
            //check if we are here because no known header that we are
            //inderested for exist.
            if (response_data[index_char] != '\r'){
                current_state = s::state_UNKNOWN_HEADER;
            } else if (response_data[index_char] == '\r' &&
                       ((index_char + 1) < bytes_transferred) &&
                       response_data[index_char + 1] == '\n'){
               index_char += 1;
               client_response.data_in_header_size = index_char - 1;
               client_response.data_in_body_size = (bytes_transferred - index_char) - 1;
               client_response.header_in.clear();
               client_response.header_in.insert(client_response.header_in.begin(),
                                                response_data.begin(),
                                                response_data.begin() + index_char + 1);
               client_response.body_in.clear();
               client_response.body_in.insert(client_response.body_in.begin(),
                                              response_data.begin() + index_char + 1,
                                              response_data.end());

               return 0;
               //current_state = state_CRLF_x2;
            } else {
                std::cout << "parse error!";
                //TODO: the request is malformed. I should return an error
            }
            continue;
        }//if (current_state == state_CRLF)

        if (response_data[index_char] == '\r' &&
                ((index_char + 2) < bytes_transferred) &&
                response_data[index_char + 1] == '\n') {
                index_char += 1;
                current_state = s::state_CRLF;
            continue;
        }

    }

}
