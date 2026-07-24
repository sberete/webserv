#include "network/client/Client.hpp"

static std::string mimeTypeForPath(std::string const& path)
{
    size_t dotPos = path.rfind('.');
    if (dotPos == std::string::npos)
        return "application/octet-stream";
    std::string extension = path.substr(dotPos);

    if (extension == ".html" || extension == ".htm") return "text/html";
    if (extension == ".css")                         return "text/css";
    if (extension == ".js")                           return "application/javascript";
    if (extension == ".json")                         return "application/json";
    if (extension == ".txt")                          return "text/plain";
    if (extension == ".xml")                          return "application/xml";
    if (extension == ".png")                          return "image/png";
    if (extension == ".jpg" || extension == ".jpeg")  return "image/jpeg";
    if (extension == ".gif")                          return "image/gif";
    if (extension == ".svg")                          return "image/svg+xml";
    if (extension == ".ico")                          return "image/x-icon";
    if (extension == ".pdf")                          return "application/pdf";
    if (extension == ".zip")                          return "application/zip";
    if (extension == ".mp4")                          return "video/mp4";
    if (extension == ".mp3")                          return "audio/mpeg";
    return "application/octet-stream";
}

void Client::_serveErrorPage(HttpStatus status, HttpResponse &response) const
{
    std::string errorPage = _config->getErrorPage(status);
    std::ifstream errorFile(errorPage.empty() ? "" : errorPage.c_str());
    std::ostringstream errorContent;
    if (errorFile.is_open())
        errorContent << errorFile.rdbuf();
    response.setStatus(status);
    response.setBody(errorContent.str());
    if (!errorPage.empty())
        response.headers["Content-Type"] = mimeTypeForPath(errorPage);
}

HttpResponse Client::_handleUpload(std::string const& filePath) const
{
    HttpResponse response;

    if (_request.path.find("..") != std::string::npos)
    {
        _serveErrorPage(HTTP_403, response);
        return response;
    }

    std::ofstream outFile(filePath.c_str(), std::ios::binary | std::ios::trunc);
    if (!outFile.is_open())
    {
        _serveErrorPage(HTTP_500, response);
        return response;
    }
    outFile.write(_request.body.data(), static_cast<std::streamsize>(_request.body.size()));
    outFile.close();

    response.setStatus(HTTP_201);
    response.headers["Content-Type"] = "text/plain";
    response.setBody("Uploaded: " + _request.path + "\n");
    return response;
}

HttpResponse Client::_handleDelete(std::string const& filePath) const
{
    HttpResponse response;

    if (_request.path.find("..") != std::string::npos)
    {
        _serveErrorPage(HTTP_403, response);
        return response;
    }

    if (std::remove(filePath.c_str()) == 0)
    {
        response.setStatus(HTTP_204);
        return response;
    }

    if (errno == ENOENT)
        _serveErrorPage(HTTP_404, response);
    else
        _serveErrorPage(HTTP_500, response);
    return response;
}

HttpResponse Client::_buildHttpResponse() const
{
    if (_request.method == "POST")
        return _handleUpload(_config->getRoot() + _request.path);
    if (_request.method == "DELETE")
        return _handleDelete(_config->getRoot() + _request.path);

    HttpResponse response;
    std::string filePath = _config->getRoot() + (_request.path == "/" ? "/" + _config->getIndex() : _request.path);
    std::ifstream requestedFile(filePath.c_str());
    if (requestedFile.is_open())
    {
        std::ostringstream fileContent;
        fileContent << requestedFile.rdbuf();
        response.setStatus(HTTP_200);
        response.setBody(fileContent.str());
        response.headers["Content-Type"] = mimeTypeForPath(filePath);
    }
    else
        _serveErrorPage(HTTP_404, response);
    return response;
}
