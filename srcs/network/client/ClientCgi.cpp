#include "network/client/Client.hpp"

bool Client::_isMethodAllowed(Location const& location, std::string const& method) const
{
    if (location.methods.empty())
        return true;
    for (size_t i = 0; i < location.methods.size(); i++)
        if (location.methods[i] == method)
            return true;
    return false;
}

bool Client::_findCgiHandler(Location const* location, std::string &outInterpreter, std::string &outScriptPath) const
{
    if (!location || location->cgiHandlers.empty())
        return false;
    size_t dotPos = _request.path.rfind('.');
    if (dotPos == std::string::npos)
        return false;
    std::string extension = _request.path.substr(dotPos);
    std::map<std::string, std::string>::const_iterator it = location->cgiHandlers.find(extension);
    if (it == location->cgiHandlers.end())
        return false;
    outInterpreter = it->second;
    outScriptPath = _config->getRoot() + _request.path;
    return true;
}

void Client::_startCgi(std::string const& interpreter, std::string const& scriptPath)
{
    _cgi = new CgiHandler();
    _cgi->start(_request, *_config, interpreter, scriptPath);
    if (_cgi->startFailed())
    {
        HttpResponse response;
        _serveErrorPage(HTTP_500, response);
        _writeBuffer = response.toString();
        delete _cgi;
        _cgi = NULL;
        _state = READY_TO_WRITE;
        return;
    }
    _state = CGI_ACTIVE;
}

void Client::_finishCgi()
{
    HttpResponse response;
    if (_cgi->timedOut())
        _serveErrorPage(HTTP_504, response);
    else
        response = _buildCgiResponse(_cgi->getRawOutput());
    _cgi->waitForProcessExit();
    delete _cgi;
    _cgi = NULL;
    _writeBuffer = response.toString();
    _state = READY_TO_WRITE;
}

HttpResponse Client::_buildCgiResponse(std::string const& rawOutput) const
{
    HttpResponse response;
    std::string separator = "\r\n\r\n";
    size_t sepLen = 4;
    size_t sepPos = rawOutput.find(separator);
    if (sepPos == std::string::npos)
    {
        separator = "\n\n";
        sepLen = 2;
        sepPos = rawOutput.find(separator);
    }
    if (sepPos == std::string::npos)
    {
        response.setStatus(HTTP_200);
        response.setBody(rawOutput);
        return response;
    }

    std::string headerBlock = rawOutput.substr(0, sepPos);
    std::string body = rawOutput.substr(sepPos + sepLen);

    int statusCode = HTTP_200;
    std::string statusText = "OK";
    size_t lineStart = 0;
    while (lineStart < headerBlock.size())
    {
        size_t lineEnd = headerBlock.find('\n', lineStart);
        if (lineEnd == std::string::npos)
            lineEnd = headerBlock.size();
        std::string line = headerBlock.substr(lineStart, lineEnd - lineStart);
        lineStart = lineEnd + 1;
        if (!line.empty() && line[line.size() - 1] == '\r')
            line.erase(line.size() - 1);
        if (line.empty())
            continue;
        size_t colonPos = line.find(':');
        if (colonPos == std::string::npos)
            continue;
        std::string key = line.substr(0, colonPos);
        std::string value = line.substr(colonPos + 1);
        while (!value.empty() && value[0] == ' ')
            value.erase(0, 1);
        if (key == "Status")
        {
            size_t spacePos = value.find(' ');
            if (spacePos == std::string::npos)
                statusCode = std::atoi(value.c_str());
            else
            {
                statusCode = std::atoi(value.substr(0, spacePos).c_str());
                statusText = value.substr(spacePos + 1);
            }
        }
        else
            response.headers[key] = value;
    }

    response.setStatus(statusCode, statusText);
    response.setBody(body);
    return response;
}

bool Client::hasCgi() const { return _cgi != NULL; }
int  Client::getCgiStdinFd() const  { return _cgi ? _cgi->getStdinFd() : -1; }
int  Client::getCgiStdoutFd() const { return _cgi ? _cgi->getStdoutFd() : -1; }
bool Client::cgiWantsToWrite() const { return _cgi != NULL && _cgi->wantsToWrite(); }

void Client::onCgiWritable()
{
    if (!_cgi)
        return;
    _cgi->onStdinWritable();
}

void Client::onCgiReadable()
{
    if (!_cgi)
        return;
    _cgi->onStdoutReadable();
    if (_cgi->isComplete())
        _finishCgi();
}

void Client::checkCgiTimeout()
{
    if (!_cgi)
        return;
    _cgi->checkTimeout();
    if (_cgi->isComplete())
        _finishCgi();
}
