#include "cgi/CgiEnvironment.hpp"

static std::string headerNameToEnvVarName(std::string const& headerName)
{
    std::string result = "HTTP_";
    for (size_t i = 0; i < headerName.size(); i++)
    {
        char c = headerName[i];
        result += (c == '-') ? '_' : static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
    }
    return result;
}

static std::string sizeToString(size_t value)
{
    std::ostringstream oss;
    oss << value;
    return oss.str();
}

std::vector<char*> toExecArray(std::vector<std::string> const& values)
{
    std::vector<char*> array;
    for (size_t i = 0; i < values.size(); i++)
        array.push_back(const_cast<char*>(values[i].c_str()));
    array.push_back(NULL);
    return array;
}

void closePipe(int pipeFds[2])
{
    close(pipeFds[0]);
    close(pipeFds[1]);
}

static void addRequestVariables(std::vector<std::string>& env, HttpRequest const& request,
                                 std::string const& scriptPath)
{
    env.push_back("REQUEST_METHOD=" + request.method);
    env.push_back("QUERY_STRING=" + request.query);
    env.push_back("SCRIPT_NAME=" + request.path);
    env.push_back("SCRIPT_FILENAME=" + scriptPath);
    env.push_back("SERVER_PROTOCOL=" + request.version);
}

static void addServerVariables(std::vector<std::string>& env, ServerConfig const& server)
{
    env.push_back("SERVER_NAME=" + server.getHost());
    env.push_back("SERVER_PORT=" + sizeToString(static_cast<size_t>(server.getPort())));
    env.push_back("GATEWAY_INTERFACE=CGI/1.1");
    env.push_back("REDIRECT_STATUS=200");
}

static void addContentVariables(std::vector<std::string>& env, HttpRequest const& request)
{
    env.push_back("CONTENT_LENGTH=" + sizeToString(request.body.size()));
    std::map<std::string, std::string>::const_iterator contentType = request.headers.find("Content-Type");
    if (contentType != request.headers.end())
        env.push_back("CONTENT_TYPE=" + contentType->second);
}

static void addHttpHeaderVariables(std::vector<std::string>& env, HttpRequest const& request)
{
    for (std::map<std::string, std::string>::const_iterator it = request.headers.begin();
         it != request.headers.end(); ++it)
        env.push_back(headerNameToEnvVarName(it->first) + "=" + it->second);
}

std::vector<std::string> buildCgiEnv(HttpRequest const& request, ServerConfig const& server,
                                      std::string const& scriptPath)
{
    std::vector<std::string> env;
    addRequestVariables(env, request, scriptPath);
    addServerVariables(env, server);
    addContentVariables(env, request);
    addHttpHeaderVariables(env, request);
    return env;
}
