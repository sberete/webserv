#ifndef CGIENVIRONMENT_HPP
    #define CGIENVIRONMENT_HPP

#include "webserv.hpp"
#include "http/HttpRequest.hpp"
#include "config/ServerConfig.hpp"

std::vector<std::string> buildCgiEnv(HttpRequest const& request, ServerConfig const& server,
                                      std::string const& scriptPath);

std::vector<char*> toExecArray(std::vector<std::string> const& values);

void closePipe(int pipeFds[2]);

#endif
