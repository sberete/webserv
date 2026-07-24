#include "network/server/Server.hpp"
#include "config/Config.hpp"

/*
** TODO — ce qu'il reste à faire sur le serveur (côté logique HTTP) :
**
** 1. DELETE — suppression de fichier
**    "DELETE" est deja valide en config (isValidMethod, default.conf a "methods GET POST DELETE")
**    mais jamais traite ensuite. Client::_buildHttpResponse (srcs/network/client/ClientResponse.cpp)
**    ne distingue que POST (upload) et le reste (lecture GET) -> une requete DELETE tombe dans la
**    branche GET et essaie de LIRE le fichier au lieu de le supprimer.
**    A faire : branche "method == DELETE" -> std::remove()/unlink() sur root+path, 200/204 si ok,
**    404 si le fichier n'existe pas, 403 si le chemin contient ".." (meme check que l'upload).
**
** 2. Listing de repertoire (autoindex)
**    La directive "autoindex on/off;" est deja parsee (ServerConfig::Location::autoindex) mais
**    jamais lue nulle part dans Client. Quand un GET tombe sur un dossier (pas un fichier) et que
**    autoindex est actif pour la location, generer une page HTML listant son contenu au lieu de 404.
**
** 3. client_max_body_size
**    Aucune limite de taille de body actuellement (Client::onReadable attend juste que
**    Content-Length soit atteint, quelle que soit sa valeur). Ajouter une directive de config +
**    une verification dans onReadable -> 413 Payload Too Large si depassee.
**
** 4. Redirection HTTP (ex. directive "return"/"redirect")
**    Pas de directive de config ni de code pour repondre 301/302 avec un header Location: vers
**    une autre URL.
**
** 5. Transfer-Encoding: chunked
**    Seul Content-Length est gere. Un client qui envoie du chunked (sans Content-Length) ne sera
**    jamais detecte comme "complet" par onReadable.
**
** 6. Bonus cookies / session
**    Rien n'existe pour l'instant (pas de parsing de "Cookie:", pas de "Set-Cookie:").
*/

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