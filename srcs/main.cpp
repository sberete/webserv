#include "network/server/Server.hpp"
#include "config/Config.hpp"

int main(int argc, char *argv[])
{
    if (argc > 2)
        return 1;

    setupSignalHandlers();

    try
    {
        Config config(argc == 2 ? argv[1] : "config/default.conf");
        Server server(config.getServers());
        server.run();
        std::cout << "webserv: shutting down" << std::endl;
    }
    catch (std::exception const& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}

// Trous mandatory restants : client_max_body_size, upload de fichier (POST hors CGI), redirection HTTP en config, Transfer-Encoding: chunked
// Point avec ton camarade — où il en est sur listing/cookies/DELETE réel