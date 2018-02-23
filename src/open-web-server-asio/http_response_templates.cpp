/* ***********************************************************************
 * Open Web Server
 * (C) 2018 by Yiannis Bourkelis (hello@andama.org)
 *
 * This file is part of Open Web Server.
 *
 * Open Web Server is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Open Web Server is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Open Web Server.  If not, see <http://www.gnu.org/licenses/>.
 * ***********************************************************************/

#include "http_response_templates.h"

/*
const QString HTTP_Response_Templates::_200_OK = ("HTTP/1.1 200 OK\r\n"
                                                    "Content-Type: %0; charset=utf-8\r\n"
                                                    "Content-Length: %1\r\n"
                                                    "\r\n");
                                                    */

const QString HTTP_Response_Templates::_200_OK_NOT_CACHED_ = ( "HTTP/1.1 200 OK\r\n"
                                                    "Server: openw/1.10.1\r\n"
                                                    "Content-Type: %0\r\n"
                                                    "Content-Length: %1\r\n"
                                                    "Connection: keep-alive\r\n"
                                                    "Accept-Ranges: bytes\r\n\r\n");



QString HTTP_Response_Templates::_200_OK_UNTIL_CONTENT_TYPE_VALUE = (
                                                    "HTTP/1.1 200 OK\r\n"
                                                    "Server: openw/1.10.1\r\n"
                                                    "Date: Wed, 21 Feb 2018 03:01:27 GMT\r\n"
                                                    "Content-Type: ");
QString HTTP_Response_Templates::_200_OK_CONTENT_LENGTH = (
                                                    "\r\nContent-Length: ");
QString HTTP_Response_Templates::_200_OK_AFTER_CONTENT_LENGTH_VALUE = (
                                                    "\r\n"
                                                    "Last-Modified: Sat, 20 Jan 2018 09:34:52 GMT\r\n"
                                                    "Connection: keep-alive\r\n"
                                                    "ETag: ""5a630d3c-6b""\r\n"
                                                    "Accept-Ranges: bytes\r\n\r\n");

std::string HTTP_Response_Templates::_200_OK_UNTIL_DATE_VALUE_ = (
                                                    "HTTP/1.1 200 OK\r\n"
                                                    "Server: openw/1.10.1\r\n"
                                                    "Date: ");
std::string HTTP_Response_Templates::_200_OK_UNTIL_CONTENT_TYPE_VALUE_ = (
                                                    "\r\n"
                                                    "Content-Type: ");
std::string HTTP_Response_Templates::_200_OK_CONTENT_LENGTH_ = (
                                                    "\r\nContent-Length: ");
std::string HTTP_Response_Templates::_200_OK_AFTER_CONTENT_LENGTH_VALUE_ = (
                                                    "\r\n"
                                                    "Last-Modified: ");
std::string HTTP_Response_Templates::_200_OK_AFTER_LAST_MODIFIED_ = (
                                                    "\r\nConnection: keep-alive\r\n"
                                                    "ETag: \"");
std::string HTTP_Response_Templates::_200_OK_AFTER_ETAG_VALUE = ("\"\r\nAccept-Ranges: bytes\r\n\r\n");

const QString HTTP_Response_Templates::_206_PARTIAL_CONTENT_RESPONSE_HEADER = ("HTTP/1.1 206 Partial Content\r\n"
                                                                                "Content-Type: %0\r\n"
                                                                                "Content-Length: %1\r\n"
                                                                                "Content-Range: bytes %2-%3/%4\r\n"
                                                                                "\r\n");

const QString HTTP_Response_Templates::_DIRECTORY_LISTING_ = ("<html><head><title>"
                                                              "Open Web Server</title>"
                                                              "</head><body><h1>"
                                                              "%0"
                                                              "</h1>"
                                                              "%1"
                                                              "</body></html>");

const QString HTTP_Response_Templates::_404_NOT_FOUND_HEADER_ = ("HTTP/1.1 404 Not Found\r\n"
                                                    "Content-Type: text/html; charset=utf-8\r\n"
                                                    "Content-Length: %0\r\n"
                                                    "\r\n");

const QString HTTP_Response_Templates::_404_NOT_FOUND_BODY_ = ("<html><head><title>404 Not Found</title></head>"
                                                         "<body bgcolor=""white"">"
                                                         "<center><h1>404 Not Found</h1></center>"
                                                         "<hr><center>Open Web Server/1.0-alpha</center>"
                                                         "</body></html>\r\n");

const QString HTTP_Response_Templates::_400_BAD_REQUEST_HEADER_ = ("HTTP/1.1 400 Bad Request\r\n"
                                                    "Content-Type: text/html; charset=utf-8\r\n"
                                                    "Content-Length: %0\r\n"
                                                    "\r\n");

const QString HTTP_Response_Templates::_400_BAD_REQUEST_BODY_ = ("<html><head><title>400 Bad Request</title></head>"
                                                         "<body bgcolor=""white"">"
                                                         "<center><h1>400 Bad Request</h1></center>"
                                                         "<hr><center>Open Web Server/1.0-alpha</center>"
                                                         "</body></html>\r\n");

HTTP_Response_Templates::HTTP_Response_Templates()
{
}

std::vector<char> HTTP_Response_Templates::merge(const QString &header, const QString &body)
{
    QString combined = header + body;
    std::string combined_str = combined.toStdString();
    return std::vector<char> (combined_str.begin(), combined_str.end());
}
