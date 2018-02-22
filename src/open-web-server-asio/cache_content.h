#ifndef CACHE_CONTENT_H
#define CACHE_CONTENT_H

#include <vector>
#include <QString>
#include <string>
#include <time.h>

class CacheContent
{
public:
    CacheContent(std::vector<char> &data_moved_, std::string &mime_type_moved_, std::string &last_modified_, std::string &etag_);

    std::vector<char> data;// the contents(bytes) of the file
    std::string mime_type;// the file mime type e.g. text/html. It is used in the response Content-Type header
    std::string last_modified; //the date/time the file was last modified e.g.: Sat, 20 Jan 2018 09:34:52 GMT . It is used in the response Last-Modified header
    std::string etag; //etag for browser caching purposes
    time_t last_access_time; //the time the file was most recently served from the cache. When the cache is full, this field is used to find the older items in the cache and remove them
};

#endif // CACHE_CONTENT_H
