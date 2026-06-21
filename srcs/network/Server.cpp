#include "network/Server.hpp"

Server::Server(const Server &other) : _serverFd(other._serverFd), _pollFds(other._pollFds), _clients(other._clients) {}

Server &Server::operator=(const Server &other)
{
    if (this != &other)
    {
        _serverFd = other._serverFd;
        _pollFds = other._pollFds;
        _clients = other._clients;
    }
    return *this;
}

Server::~Server()
{
    if (_serverFd != -1)
        close(_serverFd);
}

sockaddr_in Server::_createServerAddr() const
{
    struct sockaddr_in serverAddr = {};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8080);
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    return serverAddr;
}

void Server::_addFd(int fd, short events)
{
    struct pollfd pollEntry = {fd, events, 0};
    _pollFds.push_back(pollEntry);
}

void Server::_acceptNewClient(const pollfd &pfd)
{
    if (!(pfd.revents & POLLIN))
        return;
    int clientFd = accept(_serverFd, NULL, NULL);
    if (clientFd == -1)
        return;
    _addFd(clientFd, POLLIN);
    _clients.insert(std::make_pair(clientFd, Client(clientFd)));
}

void Server::_removeClient(std::map<int, Client>::iterator clientIt, size_t &i)
{
    close(clientIt->second.getFd());
    _clients.erase(clientIt);
    _pollFds.erase(_pollFds.begin() + i);
    i--;
}

void Server::_handleClient(size_t &i)
{
    std::map<int, Client>::iterator clientIt = _clients.find(_pollFds[i].fd);
    if (clientIt == _clients.end())
        return;

    Client &client = clientIt->second;

    if (_pollFds[i].revents & POLLIN)
        client.onReadable();
    else if (_pollFds[i].revents & POLLOUT)
        client.onWritable();

    if (client.getState() == Client::DONE)
        _removeClient(clientIt, i);
    else if (client.getState() == Client::READY_TO_WRITE)
        _pollFds[i].events = POLLOUT;
}

Server::Server() : _serverFd(-1)
{
    _serverFd = socket(AF_INET, SOCK_STREAM, 0);
    if (_serverFd == -1)
        handleError(ERR_SOCKET);

    sockaddr_in serverAddr = _createServerAddr();

    if (bind(_serverFd, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1)
        handleError(ERR_BIND, _serverFd);

    if (listen(_serverFd, LISTEN_BACKLOG) == -1)
        handleError(ERR_LISTEN, _serverFd);

    _addFd(_serverFd, POLLIN);
}

void Server::run()
{
    while (true)
    {
        int eventCount = poll(_pollFds.data(), _pollFds.size(), -1);
        if (eventCount == -1)
            handleError(ERR_POLL, _serverFd);

        for (size_t i = 0; i < _pollFds.size(); i++)
        {
            if (_pollFds[i].fd == _serverFd)
                _acceptNewClient(_pollFds[i]);
            else
                _handleClient(i);
        }
    }
}
