#ifndef CACHE_CONTENT_H
#define CACHE_CONTENT_H

#include <vector>
#include <QString>

class CacheContent
{
public:
    CacheContent(std::vector<char> &data_moved_, QString &mime_type_moved_);

    std::vector<char> data;
    QString mime_type;

};

#endif // CACHE_CONTENT_H
