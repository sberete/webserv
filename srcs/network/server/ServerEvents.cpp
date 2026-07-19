#include "network/server/Server.hpp"

void Server::_handleClientEvent(int fd, short revents)
{
    std::map<int, Client>::iterator clientIt = _clients.find(fd);
    if (clientIt == _clients.end())
        return;

    Client &client = clientIt->second;

    if (revents & POLLIN)
        client.onReadable();
    else if (revents & POLLOUT)
        client.onWritable();

    if (client.getState() == Client::DONE)
        _removeClient(clientIt);
    else if (client.getState() == Client::READY_TO_WRITE)
        _setPollEvents(fd, POLLOUT);
    else if (client.getState() == Client::CGI_ACTIVE)
        _setPollEvents(fd, 0);
}

void Server::_handleCgiEvent(int clientFd, short revents)
{
    std::map<int, Client>::iterator clientIt = _clients.find(clientFd);
    if (clientIt == _clients.end())
        return;

    Client &client = clientIt->second;

    if (revents & POLLOUT)
        client.onCgiWritable();
    if (revents & (POLLIN | POLLHUP))
        client.onCgiReadable();

    if (client.getState() == Client::READY_TO_WRITE)
        _setPollEvents(clientFd, POLLOUT);
}
