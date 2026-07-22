#include "http/HttpStatus.hpp"

const char *httpStatusText(HttpStatus status)
{
    switch (status)
    {
        case HTTP_200: return "OK";
        case HTTP_201: return "Created";
        case HTTP_403: return "Forbidden";
        case HTTP_404: return "Not Found";
        case HTTP_405: return "Method Not Allowed";
        case HTTP_500: return "Internal Server Error";
        case HTTP_504: return "Gateway Timeout";
        default:       return "Unknown";
    }
}
