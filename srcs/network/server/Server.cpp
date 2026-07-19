#include "network/server/Server.hpp"

Server::Server(const Server &other)
    : _configs(other._configs), _listenFdToConfigIndex(other._listenFdToConfigIndex),
      _pollFds(other._pollFds), _clients(other._clients) {}

Server &Server::operator=(const Server &other)
{
    if (this != &other)
    {
        _configs = other._configs;
        _listenFdToConfigIndex = other._listenFdToConfigIndex;
        _pollFds = other._pollFds;
        _clients = other._clients;
    }
    return *this;
}

Server::~Server()
{
    for (std::map<int, size_t>::const_iterator it = _listenFdToConfigIndex.begin();
         it != _listenFdToConfigIndex.end(); ++it)
        close(it->first);
}

Server::Server(std::vector<ServerConfig> const& configs) : _configs(configs)
{
    for (size_t i = 0; i < _configs.size(); i++)
    {
        int listenFd = socket(AF_INET, SOCK_STREAM, 0);
        if (listenFd == -1)
            handleError(ERR_SOCKET);

        int opt = 1;
        setsockopt(listenFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
        fcntl(listenFd, F_SETFL, O_NONBLOCK);

        sockaddr_in serverAddr = _createServerAddr(_configs[i]);

        if (bind(listenFd, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1)
            handleError(ERR_BIND, listenFd);

        if (listen(listenFd, LISTEN_BACKLOG) == -1)
            handleError(ERR_LISTEN, listenFd);

        _addFd(listenFd, POLLIN);
        _listenFdToConfigIndex[listenFd] = i;
    }
}

void Server::run()
{
    while (!g_shutdownRequested)
    {
        std::vector<struct pollfd> pollSet = _pollFds;
        std::map<int, int> cgiFdToClientFd;

        for (std::map<int, Client>::iterator it = _clients.begin(); it != _clients.end(); ++it)
        {
            Client &client = it->second;
            if (!client.hasCgi())
                continue;
            int stdinFd = client.getCgiStdinFd();
            if (stdinFd != -1 && client.cgiWantsToWrite())
            {
                struct pollfd entry = {stdinFd, POLLOUT, 0};
                pollSet.push_back(entry);
                cgiFdToClientFd[stdinFd] = it->first;
            }
            int stdoutFd = client.getCgiStdoutFd();
            if (stdoutFd != -1)
            {
                struct pollfd entry = {stdoutFd, POLLIN, 0};
                pollSet.push_back(entry);
                cgiFdToClientFd[stdoutFd] = it->first;
            }
        }

        int eventCount = poll(pollSet.data(), pollSet.size(), POLL_TIMEOUT_MS);
        if (eventCount == -1)
        {
            if (errno == EINTR)
                continue;
            handleError(ERR_POLL);
        }

        for (std::map<int, Client>::iterator it = _clients.begin(); it != _clients.end(); ++it)
        {
            it->second.checkCgiTimeout();
            if (it->second.getState() == Client::READY_TO_WRITE)
                _setPollEvents(it->first, POLLOUT);
        }

        for (size_t i = 0; i < pollSet.size(); i++)
        {
            if (pollSet[i].revents == 0)
                continue;
            int fd = pollSet[i].fd;
            std::map<int, size_t>::const_iterator listenIt = _listenFdToConfigIndex.find(fd);
            if (listenIt != _listenFdToConfigIndex.end())
                _acceptNewClient(pollSet[i], _configs[listenIt->second]);
            else if (_clients.count(fd))
                _handleClientEvent(fd, pollSet[i].revents);
            else
            {
                std::map<int, int>::iterator ownerIt = cgiFdToClientFd.find(fd);
                if (ownerIt != cgiFdToClientFd.end())
                    _handleCgiEvent(ownerIt->second, pollSet[i].revents);
            }
        }
    }
}
