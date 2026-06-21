#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "webserv.hpp"
#include "http/HttpRequest.hpp"
#include "http/HttpResponse.hpp"

class Client
{
    public:
        enum State
        {
            READING,
            READY_TO_WRITE,
            DONE
        };

        Client();
        Client(int socketFd);
        Client(const Client &other);
        Client &operator=(const Client &other);
        ~Client();

        void onReadable();
        void onWritable();

        int getFd() const;
        State getState() const;

    private:
        bool _recvFailed();
        bool _sendFailed();
        HttpResponse _buildHttpResponse() const;
        void _serve404(HttpResponse &response) const;

        int _socketFd;
        std::string _readBuffer;
        std::string _writeBuffer;
        State _state;
        HttpRequest _request;
};

#endif
