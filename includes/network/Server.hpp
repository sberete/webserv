#ifndef SERVER_HPP
    #define SERVER_HPP

#include "webserv.hpp"
#include "network/Client.hpp"

class Server
{
    public:
        Server();
        Server(const Server &other);
        Server &operator=(const Server &other);
        ~Server();

        void run();

    private:
        void _addFd(int fd, short events);
        void _acceptNewClient(const pollfd &pfd);
        void _handleClient(size_t &i);
        void _removeClient(std::map<int, Client>::iterator clientIt, size_t &i);
        sockaddr_in _createServerAddr() const;

        int _serverFd;
        std::vector<struct pollfd> _pollFds;
        std::map<int, Client> _clients;
};

#endif
