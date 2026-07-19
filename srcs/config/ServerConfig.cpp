#include "config/ServerConfig.hpp"

ServerConfig::ServerConfig() : _port(80) {}

ServerConfig::ServerConfig(ServerConfig const& other)
    : _host(other._host), _port(other._port), _root(other._root),
      _index(other._index), _errorPages(other._errorPages), _locations(other._locations) {}

ServerConfig& ServerConfig::operator=(ServerConfig const& other)
{
    if (this != &other)
    {
        _host = other._host;
        _port = other._port;
        _root = other._root;
        _index = other._index;
        _errorPages = other._errorPages;
        _locations = other._locations;
    }
    return *this;
}

ServerConfig::~ServerConfig() {}

void ServerConfig::setHost(std::string const& host)               { _host = host; }
void ServerConfig::setPort(int port)                               { _port = port; }
void ServerConfig::setRoot(std::string const& root)               { _root = root; }
void ServerConfig::setIndex(std::string const& index)             { _index = index; }
void ServerConfig::setErrorPage(int code, std::string const& page){ _errorPages[code] = page; }
void ServerConfig::addLocation(std::string const& path, Location const& location) { _locations[path] = location; }

std::string const& ServerConfig::getHost() const  
{ 
    return _host; 
}

int                ServerConfig::getPort() const  { return _port; }
std::string const& ServerConfig::getRoot() const  { return _root; }
std::string const& ServerConfig::getIndex() const { return _index; }

std::string ServerConfig::getErrorPage(int code) const
{
    std::map<int, std::string>::const_iterator it = _errorPages.find(code);
    return it != _errorPages.end() ? it->second : "";
}

Location const& ServerConfig::getLocation(std::string const& path) const
{
    return _locations.at(path);
}

bool ServerConfig::hasLocation(std::string const& path) const
{
    return _locations.find(path) != _locations.end();
}

Location const* ServerConfig::matchLocation(std::string const& path) const
{
    Location const* best = NULL;
    size_t bestLen = 0;
    for (std::map<std::string, Location>::const_iterator it = _locations.begin(); it != _locations.end(); ++it)
    {
        std::string const& locationPath = it->first;
        if (path.size() >= locationPath.size() && path.compare(0, locationPath.size(), locationPath) == 0 && locationPath.size() > bestLen)
        {
            best = &it->second;
            bestLen = locationPath.size();
        }
    }
    return best;
}
