#include "cache_content.h"

CacheContent::CacheContent(std::vector<char> &data_moved_, QString &mime_type_moved_) :
    data(std::move(data_moved_)), mime_type(std::move(mime_type_moved_))
{

}
