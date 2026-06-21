#include "http/HttpResponse.hpp"

HttpResponse::HttpResponse() : statusCode(0) {}

HttpResponse::HttpResponse(const HttpResponse &other)
    : statusCode(other.statusCode), statusText(other.statusText),
      body(other.body), headers(other.headers) {}

HttpResponse &HttpResponse::operator=(const HttpResponse &other)
{
    if (this != &other)
    {
        statusCode = other.statusCode;
        statusText = other.statusText;
        body = other.body;
        headers = other.headers;
    }
    return *this;
}

HttpResponse::~HttpResponse() {}

void HttpResponse::setStatus(HttpStatus status)
{
    statusCode = status;
    statusText = httpStatusText(status);
}

void HttpResponse::setBody(std::string content)
{
    body = content;
    std::ostringstream lengthStream;
    lengthStream << content.size();
    headers["Content-Length"] = lengthStream.str();
}

std::string HttpResponse::toString() const
{
    std::ostringstream responseStream;
    responseStream << "HTTP/1.1 " << statusCode << " " << statusText << "\r\n";
    for (std::map<std::string, std::string>::const_iterator headerIt = headers.begin(); headerIt != headers.end(); ++headerIt)
        responseStream << headerIt->first << ": " << headerIt->second << "\r\n";
    responseStream << "\r\n" << body;
    return responseStream.str();
}
