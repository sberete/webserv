#ifndef SIGNAL_HPP
    #define SIGNAL_HPP

#include "webserv.hpp"

extern volatile sig_atomic_t g_shutdownRequested;

void setupSignalHandlers();

#endif
