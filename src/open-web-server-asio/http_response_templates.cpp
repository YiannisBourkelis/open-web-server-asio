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

const QString HTTP_Response_Templates::_200_OK = ("HTTP/1.1 200 OK\r\n"
                                                    "Content-Type: %0; charset=utf-8\r\n"
                                                    "Content-Length: %1\r\n"
                                                    "\r\n");

const QString HTTP_Response_Templates::_206_PARTIAL_CONTENT_RESPONSE_HEADER = ("HTTP/1.1 206 Partial Content\r\n"
                                                                                "Content-Type: %0\r\n"
                                                                                "Content-Length: %1\r\n"
                                                                                "Content-Range: bytes %2-%3/%4\r\n"
                                                                                "\r\n");

const QString HTTP_Response_Templates::_404_ = ("HTTP/1.1 404 Not Found\r\n"
                                                    "Content-Type: %0; charset=utf-8\r\n"
                                                    "Content-Length: %1\r\n"
                                                    "\r\n");

const QString HTTP_Response_Templates::_DIRECTORY_LISTING_ = ("<html><head><title>"
                                                              "Open Web Server</title>"
                                                              "</head><body><h1>"
                                                              "%0"
                                                              "</h1>"
                                                              "%1"
                                                              "</body></html>");
const QString HTTP_Response_Templates::_404_NOT_FOUND_ = ("<html><head><title>404 Not Found</title></head>"
                                                         "<body bgcolor=""white"">"
                                                         "<center><h1>404 Not Found</h1></center>"
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
