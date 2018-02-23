#ifndef CACHE_REMOVE_H
#define CACHE_REMOVE_H

#include <unordered_map>
#include <time.h>
#include "cache_key.h"
#include "cache_content.h"
#include <QString>

class CacheRemove
{
public:
    CacheRemove(std::unordered_map<CacheKey, CacheContent>::iterator cache_iterator_,
                time_t last_access_time_,
                size_t size_,
                QString file_name_);

    bool operator<(const CacheRemove& other) const
    {
        return this->last_access_time < other.last_access_time;
    }

    std::unordered_map<CacheKey, CacheContent>::iterator cache_iterator;
    time_t last_access_time;
    size_t size;
    QString file_name;//TODO: file_name member has no usage. I used it for debugging purposes, to check if the cache cleanup works as expected by removing the older used files
};

#endif // CACHE_REMOVE_H
