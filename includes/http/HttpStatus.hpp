#ifndef HTTPSTATUS_HPP
    #define HTTPSTATUS_HPP

enum HttpStatus
{
    HTTP_200 = 200,
    HTTP_404 = 404,
    HTTP_405 = 405,
    HTTP_500 = 500,
    HTTP_504 = 504
};

const char *httpStatusText(HttpStatus status);

#endif
