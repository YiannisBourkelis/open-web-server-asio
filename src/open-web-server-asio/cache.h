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

#ifndef CACHE_H
#define CACHE_H

#include <mutex>
#include <unordered_map>
#include <QString>
#include <vector>
#include <QFileSystemWatcher>
#include <QObject>
#include "cache_key.h"
#include <QCoreApplication>
#include "server_config.h"
#include "cache_content.h"
#include <boost/asio.hpp>
#include <shared_mutex>

class Cache : public QObject
{
    Q_OBJECT
public:
    Cache(boost::asio::io_service &io_service_);


    //public variables
    //TODO: ta parakatw prepei na mpoun sto server config file
    //kai na lamvanoun times apo ekei
    long long int max_file_size = 20971520; // [default=20971520 20MB, 1048576 1MB, 104857600 100MB] megisto megethos arxeiou pou mporei na mpoei stin cache
    long long int cache_max_size = 419430400; //[default=419430400 400MB, 10485760=10MB] megisto megethos cache
    double cache_clenup_percentage = 0.20; // when the cache becomes full, the remove_older_items function is called and cache_clenup_percentage bytes of the cache_max_size bytes will be removed from the cache
    long long int cache_current_size = 0;
    QFileSystemWatcher *file_system_watcher;

    /************ THE CACHE ************/
    std::unordered_map<CacheKey, CacheContent> cached_items;
    std::shared_mutex cached_items_shared_mutex;
    /***********************************/

    void initialize(QCoreApplication *qcore_application);
    void remove_older_items();

public slots:
    void slot_fileChanged(const QString &path);

private:
    boost::asio::deadline_timer cache_timer_;
    void cache_monitor_tick(const boost::system::error_code &);
    void init_cache_monitor();
    void init_file_monitor(QCoreApplication *qcore_application);
};

#endif // CACHE_H
