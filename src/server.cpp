#include "server.h"

Server::Server() { }

std::shared_ptr<Client> Server::add_client(std::string _id)
{
    Client temp { _id, *this };
    std::shared_ptr<Client> temp2 { std::make_shared<Client>(temp) };
    clients.insert(std::pair<std::shared_ptr<Client>, double>(temp2, 5.0));
    return temp2;
}

