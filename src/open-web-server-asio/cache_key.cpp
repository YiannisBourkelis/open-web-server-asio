#include "cache_key.h"

CacheKey::CacheKey()
{

}

CacheKey::CacheKey(QString virtual_host_file_path__) : virtual_host_file_path(virtual_host_file_path__)
{
}
CacheKey::CacheKey(QString virtual_host_file_path__, QString real_file_path__) : virtual_host_file_path(virtual_host_file_path__), real_file_path(real_file_path__)
{
}
