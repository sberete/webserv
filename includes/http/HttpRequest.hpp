#ifndef REQUEST_HPP
    #define REQUEST_HPP

#include "webserv.hpp"

class HttpRequest
{
    public:
        HttpRequest();
        HttpRequest(const HttpRequest &other);
        HttpRequest &operator=(const HttpRequest &other);
        ~HttpRequest();

        void parse(const std::string &raw);

        std::string method;
        std::string path;
        std::string query;
        std::string version;
        std::string body;
        std::map<std::string, std::string> headers;
};

#endif
