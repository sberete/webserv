#ifndef RESPONSE_HPP
    #define RESPONSE_HPP

#include "webserv.hpp"
#include "http/HttpStatus.hpp"

class HttpResponse
{
    public:
        HttpResponse();
        HttpResponse(const HttpResponse &other);
        HttpResponse &operator=(const HttpResponse &other);
        ~HttpResponse();

        int statusCode;
        std::string statusText;
        std::string body;
        std::map<std::string, std::string> headers;

        void setStatus(HttpStatus status);
        void setBody(std::string content);
        std::string toString() const;
};

#endif
