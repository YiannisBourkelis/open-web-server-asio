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

#ifndef HTTP_RESPONSE_TEMPLATES_H
#define HTTP_RESPONSE_TEMPLATES_H

#include <QString>
#include <vector>
#include <string>

class HTTP_Response_Templates
{
public:
    HTTP_Response_Templates();

    //static variables
    static const QString _200_OK_NOT_CACHED_;

    static QString _200_OK_UNTIL_CONTENT_TYPE_VALUE;
    static QString _200_OK_CONTENT_LENGTH;
    static QString _200_OK_AFTER_CONTENT_LENGTH_VALUE;

    static std::string _200_OK_UNTIL_DATE_VALUE_;
    static std::string _200_OK_UNTIL_CONTENT_TYPE_VALUE_;
    static std::string _200_OK_CONTENT_LENGTH_;
    static std::string _200_OK_AFTER_CONTENT_LENGTH_VALUE_;
    static std::string _200_OK_AFTER_LAST_MODIFIED_;
    static std::string _200_OK_AFTER_ETAG_VALUE;

    static const std::string _302_FOUND_REDIRECT_HEADER_;

    static const QString _206_PARTIAL_CONTENT_RESPONSE_HEADER;
    static const QString _DIRECTORY_LISTING_;
    static const QString _404_NOT_FOUND_HEADER_;
    static const QString _404_NOT_FOUND_BODY_;
    static const QString _400_BAD_REQUEST_HEADER_;
    static const QString _400_BAD_REQUEST_BODY_;

    //static methods
    static std::vector<char> merge(const QString &header, const QString &body);
};

#endif // HTTP_RESPONSE_TEMPLATES_H
