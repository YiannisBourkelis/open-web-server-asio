#ifndef CACHE_KEY_H
#define CACHE_KEY_H

#include <QString>
//#include <QHash>

class CacheKey
{
public:
    CacheKey();
    CacheKey(std::string virtual_host_file_path__);
    CacheKey(std::string virtual_host_file_path__, QString real_file_path__);

    std::string virtual_host_file_path;
    QString real_file_path;

    bool operator==(const CacheKey& other) const
    {
        return virtual_host_file_path == other.virtual_host_file_path;
    }
    /*
    bool operator==(const QString& other) const
    {
        return virtual_host_file_path == other;
    }
    */

    bool operator<(const CacheKey& other) const
    {
        return virtual_host_file_path < other.virtual_host_file_path;
    }

};


namespace std {

  template <>
  struct hash<CacheKey>
  {
    std::size_t operator()(const CacheKey& k) const
    {
      return (std::hash<std::string>{}(k.virtual_host_file_path));
    }
  };

}


#endif // CACHE_KEY_H
