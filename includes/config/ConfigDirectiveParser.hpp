#ifndef CONFIGDIRECTIVEPARSER_HPP
    #define CONFIGDIRECTIVEPARSER_HPP

#include "webserv.hpp"
#include "config/ServerConfig.hpp"

std::string parseDirectiveValue(std::string const& rawValue, std::string const& directive);
bool        isNumber(std::string const& str);
bool        isValidMethod(std::string const& method);
void        expectOpenBrace(std::istringstream& lineStream, std::ifstream& file, std::string const& directive);

void parseHostDirective(std::istringstream& lineStream, ServerConfig& serverConfig, bool& hostSet);
void parsePortDirective(std::istringstream& lineStream, ServerConfig& serverConfig, bool& portSet);
void parseRootDirective(std::istringstream& lineStream, ServerConfig& serverConfig, bool& rootSet);
void parseIndexDirective(std::istringstream& lineStream, ServerConfig& serverConfig, bool& indexSet);
void parseErrorPageDirective(std::istringstream& lineStream, ServerConfig& serverConfig, std::set<int>& errorPageCodes);

void parseMethodsDirective(std::istringstream& lineStream, Location& location);
void parseAutoindexDirective(std::istringstream& lineStream, Location& location);

void parseCgiExtensionDirective(std::istringstream& lineStream, Location& location);

#endif
