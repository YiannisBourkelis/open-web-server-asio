#include "cache_content.h"

CacheContent::CacheContent(const std::vector<char> &data_moved_, const std::string &mime_type_moved_, const std::string &last_modified_, const std::string &etag_) : data(std::move(data_moved_)),
                                            mime_type(std::move(mime_type_moved_)), last_modified(std::move(last_modified_)), etag(std::move(etag_))
{

}

const std::string &CacheContent::get_data_size_as_string()
{
    if(data_size_as_string_.empty()){
        data_size_as_string_ = std::to_string(this->data.size());
    }
    return std::ref(data_size_as_string_);
}
