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

//Cache replacement policies
//https://en.wikipedia.org/wiki/Cache_replacement_policies

//Web Caching Basics: Terminology, HTTP Headers, and Caching Strategies
//https://www.digitalocean.com/community/tutorials/web-caching-basics-terminology-http-headers-and-caching-strategies

//13.4 Response Cacheability
//https://www.w3.org/Protocols/rfc2616/rfc2616-sec13.html#sec13.4


#include "cache.h"
#include <QCoreApplication>
#include <iostream>
#include "rocket.h"
#include <vector>
#include "cache_remove.h"


Cache::Cache(io_service &io_service_) : cache_timer_(io_service_)
{
}

//TODO: thelei veltistopoiisi
void Cache::slot_fileChanged(const QString &path)
{
    if (path == ServerConfig::config_file_path){
        //allagi sto server config

        //gia logous aplotitas afairw ola ta index files apo tin cache
        //giati mporei na yparxei stin cache p.x. localhost kai na anaferetai sto arxeio pou yparxei sto localhost/index.html
        for (auto vhost_config : ServerConfig::server_config_map){
            for (auto dir_indexes : vhost_config.second.directoryIndexes){
                for (auto it = cached_items.begin(); it != cached_items.end();){
                    if (it->first.real_file_path.endsWith(dir_indexes)){
                        it = cached_items.erase(it);
                    } else {
                        it++;
                    } //if
                }//for cache_
            }//for dir_indexes
        }//for vhost_config

        ServerConfig::parse_config_file(path);
    }else{
        //allagi se file pou yparxei stin cache
        file_system_watcher->removePath(path);
        for (auto it = cached_items.begin(); it != cached_items.end(); it++){
            if (it->first.real_file_path == path){
                cached_items.erase(it);
                break;
            }//if
        }//for
    }
}

void Cache::initialize(QCoreApplication *qcore_application){
    init_file_monitor(qcore_application);
    init_cache_monitor();
}

void Cache::init_file_monitor(QCoreApplication *qcore_application){
    file_system_watcher = new QFileSystemWatcher(qcore_application);
    this->connect(file_system_watcher,
                     SIGNAL(fileChanged(QString)),
                     this,
                     SLOT(slot_fileChanged(QString)));

    /*
    this->connect(&file_system_watcher_,
                     SIGNAL(directoryChanged(QString)),
                     this,
                     SLOT(slot_fileChanged(QString)));
*/

    //TODO: na energopoiisw to filesystemwatcher gia na lamvanw tis allages sto config file
    file_system_watcher->addPath(ServerConfig::config_file_path);
}

void Cache::remove_older_items()
{
    //for each item in the cache, we create a new etry here so that
    //we can figure out which element to remove based on the last_access_time
    std::vector<CacheRemove> items_to_remove;

    for (std::unordered_map<CacheKey, CacheContent>::iterator item_it = cached_items.begin(); item_it != cached_items.end(); ++item_it){
        items_to_remove.push_back(CacheRemove(item_it,
                                              item_it->second.last_access_time,
                                              item_it->second.data.size(),
                                              item_it->first.real_file_path));

    }

    //sort the vector based on the last_access_time field.
    //older items are are at the beginning of the vector
    std::sort(items_to_remove.begin(), items_to_remove.end());

    //remove the older items from the cache until the required
    //bytes are cleaned. The bytes to remove are calculated based
    //on the cache_clenup_percentage.
    //e.g. if the cache_max_size = 419430400 bytes (400MB) and the cache_clenup_percentage is 0.20 (20%)
    //then we need to remove 419430400 * 0.20 = 83886080 (80 MB)
    //so, the required_cache_size will be cache_current_size - 83886080 = around 320MB
    long long int required_cache_size = cache_current_size - (cache_max_size * cache_clenup_percentage);

    //begin removing the older items. Older is first.
    for (auto & item : items_to_remove){
        cache_current_size -= item.size;
        cached_items.erase(item.cache_iterator);
        //if the items removed free the required space from the cache then exit the loop
        if (cache_current_size <= required_cache_size) break; //ok we have made some space in the cache. we are done!
    }
}

//checks if there are pending events from the filesystemwatcher
//so that we can update the cache items in case a local file is deleted/modified
//and exist in the cache, or there is a change in the config file
void Cache::init_cache_monitor(){
    //call cache_monitor_tick asynchronously every 2 seconds
    boost::posix_time::time_duration tick_period(0,0,2,0);
    cache_timer_.expires_from_now(tick_period);
    cache_timer_.async_wait(boost::bind(&Cache::cache_monitor_tick, this,
                                        boost::asio::placeholders::error));
}
void Cache::cache_monitor_tick(const boost::system::error_code&){
    QCoreApplication::processEvents();
    //std::cout << "tick\r\n";
    init_cache_monitor();
}
