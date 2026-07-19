#include "network/client/Client.hpp"

void Client::_serveErrorPage(HttpStatus status, HttpResponse &response) const
{
    std::string errorPage = _config->getErrorPage(status);
    std::ifstream errorFile(errorPage.empty() ? "" : errorPage.c_str());
    std::ostringstream errorContent;
    if (errorFile.is_open())
        errorContent << errorFile.rdbuf();
    response.setStatus(status);
    response.setBody(errorContent.str());
}

HttpResponse Client::_buildHttpResponse() const
{
    HttpResponse response;
    std::string filePath = _config->getRoot() + (_request.path == "/" ? "/" + _config->getIndex() : _request.path);
    std::ifstream requestedFile(filePath.c_str());
    if (requestedFile.is_open())
    {
        std::ostringstream fileContent;
        fileContent << requestedFile.rdbuf();
        response.setStatus(HTTP_200);
        response.setBody(fileContent.str());
    }
    else
        _serveErrorPage(HTTP_404, response);
    return response;
}
