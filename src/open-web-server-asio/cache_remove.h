#ifndef CACHE_REMOVE_H
#define CACHE_REMOVE_H

#include <unordered_map>
#include <time.h>
#include "cache_key.h"
#include "cache_content.h"

class CacheRemove
{
public:
    CacheRemove(std::unordered_map<CacheKey, CacheContent>::iterator cache_iterator_,
                time_t last_access_time_,
                size_t size_);

    bool operator<(const CacheRemove& other) const
    {
        return this->last_access_time < other.last_access_time;
    }

    std::unordered_map<CacheKey, CacheContent>::iterator cache_iterator;
    time_t last_access_time;
    size_t size;
};

#endif // CACHE_REMOVE_H
