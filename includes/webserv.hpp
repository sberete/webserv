#ifndef WEBSERV_HPP
#define WEBSERV_HPP

#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <iostream>
#include <map>
#include <fstream>
#include <sstream>
#include <poll.h>
#include <fcntl.h>
#include <vector>
#include <cstdlib>
#include <cstdio>
#include <csignal>

#include "utils/error/Error.hpp"
#include "utils/print.hpp"

#define MAX_CLIENTS       100
#define LISTEN_BACKLOG    128
#define RECV_BUFFER_SIZE  4096

#endif
