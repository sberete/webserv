#ifndef HTTPSTATUS_HPP
    #define HTTPSTATUS_HPP

enum HttpStatus
{
    HTTP_200 = 200,
    HTTP_404 = 404
};

const char *httpStatusText(HttpStatus status);

#endif
