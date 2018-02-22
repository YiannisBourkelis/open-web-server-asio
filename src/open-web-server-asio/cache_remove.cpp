#include "cache_remove.h"


CacheRemove::CacheRemove(std::unordered_map<CacheKey, CacheContent>::iterator cache_iterator_,
                         time_t last_access_time_,
                         size_t size_) :
    cache_iterator(cache_iterator_),
    last_access_time(last_access_time_),
    size(size_)
{

}
