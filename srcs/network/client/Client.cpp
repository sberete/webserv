#include "network/client/Client.hpp"

Client::Client() : _socketFd(-1), _state(READING), _config(NULL), _cgi(NULL) {}

Client::Client(int socketFd, ServerConfig const* config)
    : _socketFd(socketFd), _state(READING), _config(config), _cgi(NULL) {}

Client::Client(const Client &other)
    : _socketFd(other._socketFd), _readBuffer(other._readBuffer), _writeBuffer(other._writeBuffer),
      _state(other._state), _request(other._request), _config(other._config), _cgi(other._cgi) {}

Client &Client::operator=(const Client &other)
{
    if (this != &other)
    {
        _socketFd = other._socketFd;
        _readBuffer = other._readBuffer;
        _writeBuffer = other._writeBuffer;
        _state = other._state;
        _request = other._request;
        _config = other._config;
        _cgi = other._cgi;
    }
    return *this;
}

Client::~Client() { delete _cgi; }

int Client::getFd() const
{
    return _socketFd;
}

Client::State Client::getState() const
{
    return _state;
}

bool Client::_recvFailed()
{
    char recvBuffer[RECV_BUFFER_SIZE];
    ssize_t bytesRead = recv(_socketFd, recvBuffer, sizeof(recvBuffer), 0);
    if (bytesRead > 0)
    {
        _readBuffer.append(recvBuffer, bytesRead);
        return false;
    }
    if (bytesRead == -1 && (errno == EAGAIN || errno == EWOULDBLOCK))
        return true;
    _state = DONE;
    return true;
}

bool Client::_sendFailed()
{
    ssize_t bytesWritten = send(_socketFd, _writeBuffer.c_str(), _writeBuffer.size(), 0);
    if (bytesWritten > 0)
    {
        _writeBuffer.erase(0, bytesWritten);
        return false;
    }
    if (bytesWritten == -1 && (errno == EAGAIN || errno == EWOULDBLOCK))
        return true;
    _state = DONE;
    return true;
}

static std::string toLower(std::string const& str)
{
    std::string lower = str;
    for (size_t i = 0; i < lower.size(); i++)
        lower[i] = std::tolower(static_cast<unsigned char>(lower[i]));
    return lower;
}

static size_t parseContentLength(std::string const& raw, size_t headerEnd)
{
    size_t lineStart = 0;
    while (lineStart < headerEnd)
    {
        size_t lineEnd = raw.find("\r\n", lineStart);
        if (lineEnd == std::string::npos || lineEnd > headerEnd)
            lineEnd = headerEnd;
        std::string line = raw.substr(lineStart, lineEnd - lineStart);
        size_t colonPos = line.find(':');
        if (colonPos != std::string::npos && toLower(line.substr(0, colonPos)) == "content-length")
        {
            std::string value = line.substr(colonPos + 1);
            while (!value.empty() && value[0] == ' ')
                value.erase(0, 1);
            return static_cast<size_t>(std::atoi(value.c_str()));
        }
        lineStart = lineEnd + 2;
    }
    return 0;
}

void Client::onReadable()
{
    if (_recvFailed())
        return;
    size_t headerEnd = _readBuffer.find("\r\n\r\n");
    if (headerEnd == std::string::npos)
        return;
    size_t contentLength = parseContentLength(_readBuffer, headerEnd);
    if (_readBuffer.size() < headerEnd + 4 + contentLength)
        return;

    _request.parse(_readBuffer);

    Location const* location = _config->matchLocation(_request.path);
    if (location && !_isMethodAllowed(*location, _request.method))
    {
        HttpResponse response;
        _serveErrorPage(HTTP_405, response);
        _writeBuffer = response.toString();
        _state = READY_TO_WRITE;
        return;
    }

    std::string interpreter, scriptPath;
    if (_findCgiHandler(location, interpreter, scriptPath))
    {
        _startCgi(interpreter, scriptPath);
        return;
    }

    _writeBuffer = _buildHttpResponse().toString();
    _state = READY_TO_WRITE;
}

void Client::onWritable()
{
    if (_sendFailed())
        return;
    if (_writeBuffer.empty())
        _state = DONE;
}
