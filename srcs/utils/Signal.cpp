#include "webserv.hpp"

volatile sig_atomic_t g_shutdownRequested = 0;

static void handleShutdownSignal(int signum)
{
    (void)signum;
    g_shutdownRequested = 1;
}

static void installHandler(int signum, void (*handler)(int))
{
    struct sigaction action;
    action.sa_handler = handler;
    sigemptyset(&action.sa_mask);
    action.sa_flags = SA_RESTART;
    sigaction(signum, &action, NULL);
}

void setupSignalHandlers()
{
    installHandler(SIGPIPE, SIG_IGN);
    installHandler(SIGINT, handleShutdownSignal);
    installHandler(SIGTERM, handleShutdownSignal);
    installHandler(SIGQUIT, handleShutdownSignal);
}
