#include "cache_content.h"

CacheContent::CacheContent(std::vector<char> &data_moved_, std::string &mime_type_moved_, std::string &last_modified_, std::string &etag_) : data(std::move(data_moved_)),
                                            mime_type(std::move(mime_type_moved_)), last_modified(std::move(last_modified_)), etag(std::move(etag_))
{

}
