#ifndef WEBSERV_HPP
#define WEBSERV_HPP

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <cctype>
#include <iostream>
#include <map>
#include <set>
#include <fstream>
#include <sstream>
#include <poll.h>
#include <fcntl.h>
#include <vector>
#include <cstdlib>
#include <cstdio>
#include <csignal>
#include <signal.h>
#include <cerrno>
#include <ctime>
#include <sys/wait.h>

#include "utils/error/NetworkError.hpp"
#include "utils/print.hpp"
#include "utils/Signal.hpp"

#define MAX_CLIENTS       100
#define LISTEN_BACKLOG    128
#define RECV_BUFFER_SIZE  4096
#define CGI_TIMEOUT_SECONDS 5
#define POLL_TIMEOUT_MS      1000

#endif
