#ifndef CACHE_CONTENT_H
#define CACHE_CONTENT_H

#include <vector>
#include <QString>
#include <string>
#include <time.h>

class CacheContent
{
public:
    CacheContent(const std::vector<char> &data_moved_, const std::string &mime_type_moved_, const std::string &last_modified_, const std::string &etag_);

    const std::vector<char> data;// the contents(bytes) of the file
    const std::string &get_data_size_as_string();

    const std::string mime_type;// the file mime type e.g. text/html. It is used in the response Content-Type header
    const std::string last_modified; //the date/time the file was last modified e.g.: Sat, 20 Jan 2018 09:34:52 GMT . It is used in the response Last-Modified header
    const std::string etag; //etag for browser caching purposes
    time_t last_access_time; //the time the file was most recently served from the cache. When the cache is full, this field is used to find the older items in the cache and remove them
private:
    std::string data_size_as_string_;
};

#endif // CACHE_CONTENT_H
