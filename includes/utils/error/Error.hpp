#ifndef ERROR_HPP
    #define ERROR_HPP

#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <unistd.h>

enum ErrorType
{
    ERR_SOCKET,
    ERR_BIND,
    ERR_LISTEN,
    ERR_POLL,

    ERR_ACCEPT,
    ERR_RECV,
    ERR_SEND,
};

void handleError(ErrorType type, int fd = -1);

#endif
