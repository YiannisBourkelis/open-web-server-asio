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

Cache::Cache()
{
    this->connect(&file_system_watcher,
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
    //file_system_watcher.addPath(app_path + "/" + server_config_filename);
}

//TODO: thelei veltistopoiisi
void Cache::slot_fileChanged(const QString &path)
{
    if (path == app_path + "/" + server_config_filename){
        //allagi sto server config

        //gia logous aplotitas afairw ola ta index files apo tin cache
        //giati mporei na yparxei stin cache p.x. localhost kai na anaferetai sto arxeio pou yparxei sto localhost/index.html
        for (auto vhost_config : server_config_->server_config_map){
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

        server_config_->parse_config_file(path);
    }else{
        //allagi se file pou yparxei stin cache
        file_system_watcher.removePath(path);
        for (auto it = cached_items.begin(); it != cached_items.end(); it++){
            if (it->first.real_file_path == path){
                cached_items.erase(it);
                break;
            }//if
        }//for
    }
}
