#include "network/server/Server.hpp"

sockaddr_in Server::_createServerAddr(ServerConfig const& config) const
{
    struct sockaddr_in serverAddr = {};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(config.getPort());
    serverAddr.sin_addr.s_addr = inet_addr(config.getHost().c_str());
    return serverAddr;
}

void Server::_addFd(int fd, short events)
{
    struct pollfd pollEntry = {fd, events, 0};
    _pollFds.push_back(pollEntry);
}

void Server::_acceptNewClient(const pollfd &pollFd, ServerConfig const& config)
{
    if (!(pollFd.revents & POLLIN))
        return;
    int clientFd = accept(pollFd.fd, NULL, NULL);
    if (clientFd == -1)
        return;
    fcntl(clientFd, F_SETFL, O_NONBLOCK);
    _addFd(clientFd, POLLIN);
    _clients.insert(std::make_pair(clientFd, Client(clientFd, &config)));
}

void Server::_setPollEvents(int fd, short events)
{
    for (size_t i = 0; i < _pollFds.size(); i++)
    {
        if (_pollFds[i].fd == fd)
        {
            _pollFds[i].events = events;
            return;
        }
    }
}

void Server::_removeClient(std::map<int, Client>::iterator clientIt)
{
    int fd = clientIt->second.getFd();
    close(fd);
    _clients.erase(clientIt);
    for (size_t i = 0; i < _pollFds.size(); i++)
    {
        if (_pollFds[i].fd == fd)
        {
            _pollFds.erase(_pollFds.begin() + i);
            break;
        }
    }
}
