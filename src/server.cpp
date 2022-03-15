#include "server.h"

Server::Server() { }

std::shared_ptr<Client> Server::add_client(std::string _id)
{
    Client temp { _id, *this };
    std::shared_ptr<Client> temp2 { std::make_shared<Client>(temp) };
    clients.insert(std::pair<std::shared_ptr<Client>, double>(temp2, 5.0));
    return temp2;
}

std::shared_ptr<Client> Server::get_client(std::string id)
{
    for (auto x : clients) {
        if (x.first->get_id() == id) {
            return x.first;
        }
    }

    throw std::logic_error("There is no such id");
}
