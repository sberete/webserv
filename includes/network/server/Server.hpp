#ifndef SERVER_HPP
    #define SERVER_HPP

#include "webserv.hpp"
#include "config/ServerConfig.hpp"
#include "network/client/Client.hpp"

class Server
{
    public:
        Server(std::vector<ServerConfig> const& configs);
        Server(const Server &other);
        Server &operator=(const Server &other);
        ~Server();

        void run();

    private:
        void _addFd(int fd, short events);
        void _acceptNewClient(const pollfd &pollFd, ServerConfig const& config);
        void _handleClientEvent(int fd, short revents);
        void _handleCgiEvent(int clientFd, short revents);
        void _removeClient(std::map<int, Client>::iterator clientIt);
        void _setPollEvents(int fd, short events);
        sockaddr_in _createServerAddr(ServerConfig const& config) const;

        std::vector<ServerConfig> _configs;
        std::map<int, size_t> _listenFdToConfigIndex;
        std::vector<struct pollfd> _pollFds;
        std::map<int, Client> _clients;
};

#endif
