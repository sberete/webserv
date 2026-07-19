#include "config/ConfigDirectiveParser.hpp"
#include "utils/error/ConfigError.hpp"

std::string parseDirectiveValue(std::string const& rawValue, std::string const& directive)
{
    if (rawValue.empty() || rawValue[rawValue.size() - 1] != ';')
        throwConfigError(ERR_CFG_MISSING_SEMICOLON, directive);
    std::string value = rawValue.substr(0, rawValue.size() - 1);
    if (value.empty())
        throwConfigError(ERR_CFG_EMPTY_VALUE, directive);
    return value;
}

bool isNumber(std::string const& str)
{
    if (str.empty())
        return false;
    for (size_t i = 0; i < str.size(); i++)
        if (!std::isdigit(str[i]))
            return false;
    return true;
}

bool isValidMethod(std::string const& method)
{
    return method == "GET" || method == "POST" || method == "DELETE";
}

void expectOpenBrace(std::istringstream& lineStream, std::ifstream& file, std::string const& directive)
{
    std::string token;
    if (lineStream >> token && token == "{")
        return;
    std::string line;
    while (std::getline(file, line))
    {
        std::istringstream nextLineStream(line);
        if (nextLineStream >> token && token == "{")
            return;
    }
    throwConfigError(ERR_CFG_MISSING_OPEN_BRACE, directive);
}

void parseHostDirective(std::istringstream& lineStream, ServerConfig& serverConfig, bool& hostSet)
{
    if (hostSet)
        throwConfigError(ERR_CFG_DUPLICATE_DIRECTIVE, "host");
    std::string value;
    lineStream >> value;
    serverConfig.setHost(parseDirectiveValue(value, "host"));
    hostSet = true;
}

void parsePortDirective(std::istringstream& lineStream, ServerConfig& serverConfig, bool& portSet)
{
    if (portSet)
        throwConfigError(ERR_CFG_DUPLICATE_DIRECTIVE, "port");
    std::string value;
    lineStream >> value;
    value = parseDirectiveValue(value, "port");
    if (!isNumber(value))
        throwConfigError(ERR_CFG_INVALID_PORT, value);
    int port = std::atoi(value.c_str());
    if (port < 1 || port > 65535)
        throwConfigError(ERR_CFG_PORT_RANGE, value);
    serverConfig.setPort(port);
    portSet = true;
}

void parseRootDirective(std::istringstream& lineStream, ServerConfig& serverConfig, bool& rootSet)
{
    if (rootSet)
        throwConfigError(ERR_CFG_DUPLICATE_DIRECTIVE, "root");
    std::string value;
    lineStream >> value;
    serverConfig.setRoot(parseDirectiveValue(value, "root"));
    rootSet = true;
}

void parseIndexDirective(std::istringstream& lineStream, ServerConfig& serverConfig, bool& indexSet)
{
    if (indexSet)
        throwConfigError(ERR_CFG_DUPLICATE_DIRECTIVE, "index");
    std::string value;
    lineStream >> value;
    serverConfig.setIndex(parseDirectiveValue(value, "index"));
    indexSet = true;
}

void parseErrorPageDirective(std::istringstream& lineStream, ServerConfig& serverConfig, std::set<int>& errorPageCodes)
{
    std::string statusCode, pagePath;
    lineStream >> statusCode >> pagePath;
    if (pagePath.empty())
    {
        if (!statusCode.empty() && statusCode[statusCode.size() - 1] == ';')
            statusCode = statusCode.substr(0, statusCode.size() - 1);
        throwConfigError(ERR_CFG_MISSING_ERROR_PAGE, statusCode);
    }
    if (!isNumber(statusCode))
        throwConfigError(ERR_CFG_INVALID_CODE, statusCode);
    int code = std::atoi(statusCode.c_str());
    if (code < 100 || code > 599)
        throwConfigError(ERR_CFG_CODE_RANGE, statusCode);
    if (!errorPageCodes.insert(code).second)
        throwConfigError(ERR_CFG_DUPLICATE_DIRECTIVE, "error_page " + statusCode);
    serverConfig.setErrorPage(code, parseDirectiveValue(pagePath, "error_page"));
}

void parseMethodsDirective(std::istringstream& lineStream, Location& location)
{
    std::string method;
    std::string lastToken;
    while (lineStream >> method)
    {
        if (!lastToken.empty())
        {
            if (!isValidMethod(lastToken))
                throwConfigError(ERR_CFG_INVALID_METHOD, lastToken);
            location.methods.push_back(lastToken);
        }
        lastToken = method;
    }
    if (lastToken.empty())
        throwConfigError(ERR_CFG_EMPTY_METHODS);
    lastToken = parseDirectiveValue(lastToken, "methods");
    if (!isValidMethod(lastToken))
        throwConfigError(ERR_CFG_INVALID_METHOD, lastToken);
    location.methods.push_back(lastToken);
}

void parseAutoindexDirective(std::istringstream& lineStream, Location& location)
{
    std::string value;
    lineStream >> value;
    location.autoindex = (parseDirectiveValue(value, "autoindex") == "on");
}

void parseCgiExtensionDirective(std::istringstream& lineStream, Location& location)
{
    std::string extension, interpreter;
    lineStream >> extension >> interpreter;
    if (interpreter.empty())
        throwConfigError(ERR_CFG_MISSING_CGI_PATH, extension);
    if (extension.empty() || extension[0] != '.')
        throwConfigError(ERR_CFG_INVALID_CGI_EXTENSION, extension);
    if (location.cgiHandlers.find(extension) != location.cgiHandlers.end())
        throwConfigError(ERR_CFG_DUPLICATE_DIRECTIVE, "cgi_extension " + extension);
    location.cgiHandlers[extension] = parseDirectiveValue(interpreter, "cgi_extension");
}
