#include "cgi/CgiHandler.hpp"
#include "cgi/CgiEnvironment.hpp"

CgiHandler::CgiHandler() : _pid(-1), _stdinFd(-1), _stdoutFd(-1), _inputOffset(0),
      _readDone(false), _timedOut(false), _startFailed(false), _startTime(0) {}

CgiHandler::~CgiHandler()
{
    _closeStdin();
    _closeStdout();
    if (_pid > 0)
    {
        kill(_pid, SIGKILL);
        int status;
        waitpid(_pid, &status, 0);
    }
}

void CgiHandler::_closeStdin()
{
    if (_stdinFd != -1)
    {
        close(_stdinFd);
        _stdinFd = -1;
    }
}

void CgiHandler::_closeStdout()
{
    if (_stdoutFd != -1)
    {
        close(_stdoutFd);
        _stdoutFd = -1;
    }
}

bool CgiHandler::_createPipes(int stdinPipe[2], int stdoutPipe[2])
{
    if (pipe(stdinPipe) == -1)
    {
        _startFailed = true;
        return false;
    }
    if (pipe(stdoutPipe) == -1)
    {
        closePipe(stdinPipe);
        _startFailed = true;
        return false;
    }
    return true;
}

void CgiHandler::_runChildProcess(HttpRequest const& request, ServerConfig const& server,
                                   std::string const& interpreter, std::string const& scriptPath,
                                   int stdinPipe[2], int stdoutPipe[2]) const
{
    dup2(stdinPipe[0], STDIN_FILENO);
    dup2(stdoutPipe[1], STDOUT_FILENO);
    closePipe(stdinPipe);
    closePipe(stdoutPipe);

    std::vector<std::string> argvStrings;
    argvStrings.push_back(interpreter);
    argvStrings.push_back(scriptPath);
    std::vector<char*> argv = toExecArray(argvStrings);

    std::vector<std::string> envStrings = buildCgiEnv(request, server, scriptPath);
    std::vector<char*> envp = toExecArray(envStrings);

    execve(interpreter.c_str(), &argv[0], &envp[0]);

    std::string failMessage = "Status: 500 Internal Server Error\r\n\r\n";
    ssize_t writeResult = write(STDOUT_FILENO, failMessage.c_str(), failMessage.size());
    (void)writeResult;
    _exit(1);
}

void CgiHandler::_setupParentAfterFork(pid_t pid, int stdinPipe[2], int stdoutPipe[2], std::string const& body)
{
    close(stdinPipe[0]);
    close(stdoutPipe[1]);
    _stdinFd = stdinPipe[1];
    _stdoutFd = stdoutPipe[0];
    fcntl(_stdinFd, F_SETFL, O_NONBLOCK);
    fcntl(_stdoutFd, F_SETFL, O_NONBLOCK);
    _pid = pid;
    _inputBuffer = body;
    _inputOffset = 0;
    _startTime = time(NULL);

    if (_inputBuffer.empty())
        _closeStdin();
}

void CgiHandler::start(HttpRequest const& request, ServerConfig const& server,
                        std::string const& interpreter, std::string const& scriptPath)
{
    int stdinPipe[2];
    int stdoutPipe[2];
    if (!_createPipes(stdinPipe, stdoutPipe))
        return;

    pid_t pid = fork();
    if (pid == -1)
    {
        closePipe(stdinPipe);
        closePipe(stdoutPipe);
        _startFailed = true;
        return;
    }

    if (pid == 0)
        _runChildProcess(request, server, interpreter, scriptPath, stdinPipe, stdoutPipe);

    _setupParentAfterFork(pid, stdinPipe, stdoutPipe, request.body);
}

int  CgiHandler::getStdinFd() const  { return _stdinFd; }
int  CgiHandler::getStdoutFd() const { return _stdoutFd; }
bool CgiHandler::wantsToWrite() const { return _stdinFd != -1; }

void CgiHandler::onStdinWritable()
{
    if (_stdinFd == -1)
        return;
    ssize_t written = write(_stdinFd, _inputBuffer.c_str() + _inputOffset, _inputBuffer.size() - _inputOffset);
    if (written <= 0)
    {
        _closeStdin();
        return;
    }
    _inputOffset += written;
    if (_inputOffset >= _inputBuffer.size())
        _closeStdin();
}

void CgiHandler::onStdoutReadable()
{
    if (_stdoutFd == -1)
        return;
    char buffer[RECV_BUFFER_SIZE];
    ssize_t bytesRead = read(_stdoutFd, buffer, sizeof(buffer));
    if (bytesRead <= 0)
    {
        _closeStdout();
        _readDone = true;
        return;
    }
    _outputBuffer.append(buffer, bytesRead);
}

bool CgiHandler::isComplete() const  { return _readDone || _startFailed || _timedOut; }
bool CgiHandler::startFailed() const { return _startFailed; }
bool CgiHandler::timedOut() const    { return _timedOut; }
std::string const& CgiHandler::getRawOutput() const { return _outputBuffer; }

void CgiHandler::checkTimeout()
{
    if (_readDone || _timedOut || _pid <= 0)
        return;
    if (time(NULL) - _startTime > CGI_TIMEOUT_SECONDS)
    {
        kill(_pid, SIGKILL);
        int status;
        waitpid(_pid, &status, 0);
        _pid = -1;
        _closeStdin();
        _closeStdout();
        _timedOut = true;
        _readDone = true;
    }
}

void CgiHandler::waitForProcessExit()
{
    if (_pid > 0)
    {
        int status;
        waitpid(_pid, &status, 0);
        _pid = -1;
    }
}
