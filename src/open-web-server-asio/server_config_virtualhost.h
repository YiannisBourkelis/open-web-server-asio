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

#ifndef SERVER_CONFIG_VIRTUALHOST_H
#define SERVER_CONFIG_VIRTUALHOST_H

#include <QString>
#include <vector>

class ServerConfigVirtualHost
{
public:
    ServerConfigVirtualHost();

    //variables
    QString DocumentRoot; //to path opou vrisketai to site p.x. /Users/userx/www/localhost/public_html/
    std::vector<QString> ServerName; // p.x. www.localhost
    std::vector<QString> directoryIndexes;
    QString directoryIndex;  // p.x. index.php index.html index.htm
};

#endif // SERVER_CONFIG_VIRTUALHOST_H
