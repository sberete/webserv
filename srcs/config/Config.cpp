#include "config/Config.hpp"
#include "config/ConfigDirectiveParser.hpp"
#include "utils/error/ConfigError.hpp"

Config::Config(std::string const& path)
{
    std::ifstream file(path.c_str());
    if (!file.is_open())
        throwConfigError(ERR_CFG_CANNOT_OPEN, path);
    _parse(file);
    if (_servers.empty())
        throwConfigError(ERR_CFG_MISSING_SERVER, path);
}

Config::Config(Config const& other) : _servers(other._servers) {}

Config& Config::operator=(Config const& other)
{
    if (this != &other)
        _servers = other._servers;
    return *this;
}

Config::~Config() {}

std::vector<ServerConfig> const& Config::getServers() const { return _servers; }

void Config::_parse(std::ifstream& file)
{
    std::string line;
    while (std::getline(file, line))
    {
        std::istringstream lineStream(line);
        std::string keyword;
        if (!(lineStream >> keyword) || keyword[0] == '#')
            continue;
        if (keyword == "server")
        {
            expectOpenBrace(lineStream, file, "server");
            _parseServer(file);
        }
        else
            throwConfigError(ERR_CFG_UNKNOWN_DIRECTIVE, keyword);
    }
}

void Config::_parseLocationDirective(std::istringstream& lineStream, std::ifstream& file, ServerConfig& serverConfig)
{
    std::string path;
    if (!(lineStream >> path))
        throwConfigError(ERR_CFG_MISSING_PATH);
    if (path[0] != '/')
        throwConfigError(ERR_CFG_INVALID_PATH, path);
    if (serverConfig.hasLocation(path))
        throwConfigError(ERR_CFG_DUPLICATE_DIRECTIVE, "location " + path);
    expectOpenBrace(lineStream, file, "location " + path);
    Location location;
    _parseLocation(file, location, path);
    serverConfig.addLocation(path, location);
}

void Config::_parseServer(std::ifstream& file)
{
    ServerConfig serverConfig;
    bool closed = false;
    bool hostSet = false;
    bool portSet = false;
    bool rootSet = false;
    bool indexSet = false;
    std::set<int> errorPageCodes;
    std::string line;
    while (std::getline(file, line))
    {
        std::istringstream lineStream(line);
        std::string keyword;
        if (!(lineStream >> keyword) || keyword[0] == '#')
            continue;
        if (keyword == "}")
        {
            closed = true;
            break;
        }
        if (keyword == "host")
            parseHostDirective(lineStream, serverConfig, hostSet);
        else if (keyword == "port")
            parsePortDirective(lineStream, serverConfig, portSet);
        else if (keyword == "root")
            parseRootDirective(lineStream, serverConfig, rootSet);
        else if (keyword == "index")
            parseIndexDirective(lineStream, serverConfig, indexSet);
        else if (keyword == "error_page")
            parseErrorPageDirective(lineStream, serverConfig, errorPageCodes);
        else if (keyword == "location")
            _parseLocationDirective(lineStream, file, serverConfig);
        else
            throwConfigError(ERR_CFG_UNKNOWN_DIRECTIVE, keyword);
    }
    if (!closed)
        throwConfigError(ERR_CFG_UNCLOSED_SERVER);
    if (serverConfig.getHost().empty())
        throwConfigError(ERR_CFG_MISSING_DIRECTIVE, "host");
    if (serverConfig.getRoot().empty())
        throwConfigError(ERR_CFG_MISSING_DIRECTIVE, "root");
    if (serverConfig.getIndex().empty())
        throwConfigError(ERR_CFG_MISSING_DIRECTIVE, "index");
    _servers.push_back(serverConfig);
}

void Config::_parseLocation(std::ifstream& file, Location& location, std::string const& path)
{
    bool closed = false;
    std::string line;
    while (std::getline(file, line))
    {
        std::istringstream lineStream(line);
        std::string keyword;
        if (!(lineStream >> keyword) || keyword[0] == '#')
            continue;
        if (keyword == "}")
        {
            closed = true;
            break;
        }
        if (keyword == "methods")
            parseMethodsDirective(lineStream, location);
        else if (keyword == "autoindex")
            parseAutoindexDirective(lineStream, location);
        else if (keyword == "cgi_extension")
            parseCgiExtensionDirective(lineStream, location);
        else
            throwConfigError(ERR_CFG_UNKNOWN_DIRECTIVE, keyword);
    }
    if (!closed)
        throwConfigError(ERR_CFG_UNCLOSED_LOCATION, path);
}
