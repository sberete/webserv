#ifndef SERVERCONFIG_HPP
    #define SERVERCONFIG_HPP

#include "webserv.hpp"

struct Location
{
    std::vector<std::string>           methods;
    bool                                autoindex;
    std::map<std::string, std::string> cgiHandlers;

    Location() : autoindex(false) {}
};

class ServerConfig
{
    public:
        ServerConfig();
        ServerConfig(ServerConfig const& other);
        ServerConfig& operator=(ServerConfig const& other);
        ~ServerConfig();

        void    setHost(std::string const& host);
        void    setPort(int port);
        void    setRoot(std::string const& root);
        void    setIndex(std::string const& index);
        void    setErrorPage(int code, std::string const& page);
        void    addLocation(std::string const& path, Location const& location);

        std::string const&  getHost() const;
        int                 getPort() const;
        std::string const&  getRoot() const;
        std::string const&  getIndex() const;
        std::string         getErrorPage(int code) const;
        Location const&     getLocation(std::string const& path) const;
        bool                hasLocation(std::string const& path) const;
        Location const*     matchLocation(std::string const& path) const;

    private:
        std::string                     _host;
        int                             _port;
        std::string                     _root;
        std::string                     _index;
        std::map<int, std::string>      _errorPages;
        std::map<std::string, Location> _locations;
};

#endif
