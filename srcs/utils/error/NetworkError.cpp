#include "utils/error/NetworkError.hpp"

static const char *getErrorMessage(ErrorType type)
{
    switch (type)
    {
        case ERR_SOCKET: return "socket";
        case ERR_BIND:   return "bind";
        case ERR_LISTEN: return "listen";
        case ERR_POLL:   return "poll";
        case ERR_ACCEPT: return "accept";
        case ERR_RECV:   return "recv";
        case ERR_SEND:   return "send";
        default:         return "unknown";
    }
}

void handleError(ErrorType type, int fd)
{
    switch (type)
    {
        case ERR_SOCKET:
        case ERR_BIND:
        case ERR_LISTEN:
        case ERR_POLL:
            if (fd != -1)
                close(fd);
            perror(getErrorMessage(type));
            exit(EXIT_FAILURE);

        default:
            std::cerr << "Error: " << getErrorMessage(type) << std::endl;
            if (fd != -1)
                close(fd);
            break;
    }
}
