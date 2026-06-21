#include "network/Client.hpp"

Client::Client() : _socketFd(-1), _state(READING) {}

Client::Client(int socketFd) : _socketFd(socketFd), _state(READING) {}

Client::Client(const Client &other)
    : _socketFd(other._socketFd), _readBuffer(other._readBuffer), _writeBuffer(other._writeBuffer),
      _state(other._state), _request(other._request) {}

Client &Client::operator=(const Client &other)
{
    if (this != &other)
    {
        _socketFd = other._socketFd;
        _readBuffer = other._readBuffer;
        _writeBuffer = other._writeBuffer;
        _state = other._state;
        _request = other._request;
    }
    return *this;
}

Client::~Client() {}

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
    ssize_t nbRead = recv(_socketFd, recvBuffer, sizeof(recvBuffer), 0);
    if (nbRead <= 0)
    {
        _state = DONE;
        return true;
    }
    _readBuffer.append(recvBuffer, nbRead);
    return false;
}

bool Client::_sendFailed()
{
    ssize_t nbWrite = send(_socketFd, _writeBuffer.c_str(), _writeBuffer.size(), 0);
    if (nbWrite <= 0)
    {
        _state = DONE;
        return true;
    }
    _writeBuffer.erase(0, nbWrite);
    return false;
}

void Client::_serve404(HttpResponse &response) const
{
    std::ifstream notFoundFile("www/404.html");
    std::ostringstream notFoundContent;
    if (notFoundFile.is_open())
        notFoundContent << notFoundFile.rdbuf();
    response.setStatus(HTTP_404);
    response.setBody(notFoundContent.str());
}

HttpResponse Client::_buildHttpResponse() const
{
    HttpResponse response;
    std::string filePath = "www" + (_request.path == "/" ? "/index.html" : _request.path);
    std::ifstream requestedFile(filePath.c_str());
    if (requestedFile.is_open())
    {
        std::ostringstream fileContent;
        fileContent << requestedFile.rdbuf();
        response.setStatus(HTTP_200);
        response.setBody(fileContent.str());
    }
    else
        _serve404(response);
    return response;
}

void Client::onReadable()
{
    if (_recvFailed())
        return;
    if (_readBuffer.find("\r\n\r\n") == std::string::npos)
        return;

    _request.parse(_readBuffer);
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
