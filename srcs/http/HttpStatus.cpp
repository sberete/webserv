#include "http/HttpStatus.hpp"

const char *httpStatusText(HttpStatus status)
{
    switch (status)
    {
        case HTTP_200: return "OK";
        case HTTP_404: return "Not Found";
        default:       return "Unknown";
    }
}
