#include "http/HttpRequest.hpp"

HttpRequest::HttpRequest() {}

HttpRequest::HttpRequest(const HttpRequest &other)
    : method(other.method), path(other.path), query(other.query), version(other.version),
      body(other.body), headers(other.headers) {}

HttpRequest &HttpRequest::operator=(const HttpRequest &other)
{
    if (this != &other)
    {
        method = other.method;
        path = other.path;
        query = other.query;
        version = other.version;
        body = other.body;
        headers = other.headers;
    }
    return *this;
}

HttpRequest::~HttpRequest() {}

void HttpRequest::parse(const std::string &rawHttpRequest)
{
    size_t currentPos = 0;
    size_t lineEnd = rawHttpRequest.find("\r\n");

    std::string requestLine = rawHttpRequest.substr(0, lineEnd);
    size_t firstSpace = requestLine.find(' ');
    size_t secondSpace = requestLine.find(' ', firstSpace + 1);

    this->method  = requestLine.substr(0, firstSpace);
    std::string target = requestLine.substr(firstSpace + 1, secondSpace - firstSpace - 1);
    this->version = requestLine.substr(secondSpace + 1);

    size_t queryPos = target.find('?');
    if (queryPos == std::string::npos)
        this->path = target;
    else
    {
        this->path = target.substr(0, queryPos);
        this->query = target.substr(queryPos + 1);
    }

    currentPos = lineEnd + 2;
    while (true)
    {
        lineEnd = rawHttpRequest.find("\r\n", currentPos);
        std::string headerLine = rawHttpRequest.substr(currentPos, lineEnd - currentPos);
        if (headerLine.empty())
        {
            currentPos = lineEnd + 2;
            break;
        }
        size_t colonPos = headerLine.find(':');
        std::string headerKey = headerLine.substr(0, colonPos);
        std::string headerValue = headerLine.substr(colonPos + 1);
        if (!headerValue.empty() && headerValue[0] == ' ')
            headerValue.erase(0, 1);
        this->headers[headerKey] = headerValue;
        currentPos = lineEnd + 2;
    }
    this->body = rawHttpRequest.substr(currentPos);
}
