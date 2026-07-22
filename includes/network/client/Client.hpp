#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "webserv.hpp"
#include "config/ServerConfig.hpp"
#include "http/HttpRequest.hpp"
#include "http/HttpResponse.hpp"
#include "cgi/CgiHandler.hpp"

class Client
{
    public:
        enum State
        {
            READING,
            CGI_ACTIVE,
            READY_TO_WRITE,
            DONE
        };

        Client();
        Client(int socketFd, ServerConfig const* config);
        Client(const Client &other);
        Client &operator=(const Client &other);
        ~Client();

        void onReadable();
        void onWritable();

        bool hasCgi() const;
        int  getCgiStdinFd() const;
        int  getCgiStdoutFd() const;
        bool cgiWantsToWrite() const;
        void onCgiWritable();
        void onCgiReadable();
        void checkCgiTimeout();

        int getFd() const;
        State getState() const;

    private:
        bool _recvFailed();
        bool _sendFailed();
        HttpResponse _buildHttpResponse() const;
        HttpResponse _handleUpload(std::string const& filePath) const;
        void _serveErrorPage(HttpStatus status, HttpResponse &response) const;
        bool _isMethodAllowed(Location const& location, std::string const& method) const;
        bool _findCgiHandler(Location const* location, std::string &outInterpreter, std::string &outScriptPath) const;
        void _startCgi(std::string const& interpreter, std::string const& scriptPath);
        void _finishCgi();
        HttpResponse _buildCgiResponse(std::string const& rawOutput) const;

        int _socketFd;
        std::string _readBuffer;
        std::string _writeBuffer;
        State _state;
        HttpRequest _request;
        ServerConfig const *_config;
        CgiHandler *_cgi;
};

#endif
