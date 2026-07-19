#ifndef CONFIG_HPP
    #define CONFIG_HPP

#include "webserv.hpp"
#include "config/ServerConfig.hpp"

class Config
{
    public:
        Config(std::string const& path);
        Config(Config const& other);
        Config& operator=(Config const& other);
        ~Config();

        std::vector<ServerConfig> const& getServers() const;

    private:
        void    _parse(std::ifstream& file);
        void    _parseServer(std::ifstream& file);
        void    _parseLocation(std::ifstream& file, Location& location, std::string const& path);
        void    _parseLocationDirective(std::istringstream& lineStream, std::ifstream& file, ServerConfig& serverConfig);

        std::vector<ServerConfig> _servers;
};

#endif
