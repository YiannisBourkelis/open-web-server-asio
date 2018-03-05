#ifndef PARSER_ENUMS_H
#define PARSER_ENUMS_H

namespace enums {

    enum http_parser_state {
        //GET /index.html HTTP/1.1
        state_start = 0,

        state_GET_G,
        state_GET_E,
        state_GET_T,

        state_POST_P,
        state_POST_O,
        state_POST_S,
        state_POST_T,


        state_URI_CONTENT,
        state_URI_CONTENT_END,
        state_URI_QUERYSTRING_CONTENT,
        state_URI_QUERYSTRING_CONTENT_END,
        state_QUERYSTRING_IN_BODY_CONTENT_BEGIN,
        state_QUERYSTRING_IN_BODY_CONTENT,

        state_SPACE_AFTER_METHOD,

        state_HTTP_H,
        state_HTTP_T,
        state_HTTP_T_T,
        state_HTTP_P,
        state_HTTP_SLASH,
        state_HTTP_VERSION_MAJOR_CONTENT,
        state_HTTP_VERSION_DOT_CONTENT,
        state_HTTP_VERSION_MINOR_CONTENT,

        state_HOST_H,
        state_HOST_o,
        state_HOST_s,
        state_HOST_t,
        state_HOST_COLON,
        state_HOST_CONTENT,
        state_HOST_CONTENT_END,

        state_HEADER_C,

        state_CONTENT_LENGTH_COLON,
        state_CONTENT_LENGTH_CONTENT,


        state_DONE,
        state_CR,
        state_CRLF,
        state_CRLF_CR,
        state_CRLF_CRLF,
        state_UNKNOWN_HEADER
    };

    enum class http_parser_result {
        success,
        fail,
        incomplete
    };

}//namespace

#endif // PARSER_ENUMS_H
