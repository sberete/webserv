#ifndef ERROR_HPP
    #define ERROR_HPP

#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <unistd.h>

enum ErrorType
{
    // fatales → perror + exit
    ERR_SOCKET,
    ERR_BIND,
    ERR_LISTEN,
    ERR_POLL,

    // non-fatales → log (+ close fd si fourni)
    ERR_ACCEPT,
    ERR_RECV,
    ERR_SEND,
};

void handleError(ErrorType type, int fd = -1);

#endif
