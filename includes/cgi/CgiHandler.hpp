#ifndef CGIHANDLER_HPP
    #define CGIHANDLER_HPP

#include "webserv.hpp"
#include "http/HttpRequest.hpp"
#include "config/ServerConfig.hpp"

class CgiHandler
{
    public:
        CgiHandler();
        ~CgiHandler();

        void start(HttpRequest const& request, ServerConfig const& server,
                    std::string const& interpreter, std::string const& scriptPath);

        int  getStdinFd() const;
        int  getStdoutFd() const;
        bool wantsToWrite() const;

        void onStdinWritable();
        void onStdoutReadable();

        bool isComplete() const;
        bool startFailed() const;
        bool timedOut() const;
        std::string const& getRawOutput() const;

        void checkTimeout();
        void waitForProcessExit();

    private:
        CgiHandler(CgiHandler const& other);
        CgiHandler& operator=(CgiHandler const& other);

        void _closeStdin();
        void _closeStdout();
        bool _createPipes(int stdinPipe[2], int stdoutPipe[2]);
        void _runChildProcess(HttpRequest const& request, ServerConfig const& server,
                                std::string const& interpreter, std::string const& scriptPath,
                                int stdinPipe[2], int stdoutPipe[2]) const;
        void _setupParentAfterFork(pid_t pid, int stdinPipe[2], int stdoutPipe[2], std::string const& body);

        pid_t       _pid;
        int         _stdinFd;
        int         _stdoutFd;
        std::string _inputBuffer;
        size_t      _inputOffset;
        std::string _outputBuffer;
        bool        _readDone;
        bool        _timedOut;
        bool        _startFailed;
        time_t      _startTime;
};

#endif
