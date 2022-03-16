#include "server.h"

Server::Server() { }

std::shared_ptr<Client> Server::add_client(std::string _id)
{
    for (auto x : clients) {
        if (x.first->get_id() == _id) {
            for (size_t i {}; i < 4; i++) {
                std::random_device random_device; // define random generator
                std::mt19937 engine { random_device() };
                std::uniform_real_distribution<double> dist(0, 9.99);
                _id = _id + std::to_string(static_cast<int>(dist(engine)));
            }
        }
    }
    Client temp { _id, *this };
    std::shared_ptr<Client> temp2 { std::make_shared<Client>(temp) };
    clients.insert(std::pair<std::shared_ptr<Client>, double>(temp2, 5.0));
    return temp2;
}

std::shared_ptr<Client> Server::get_client(std::string id) const
{
    for (auto x : clients) {
        if (x.first->get_id() == id) {
            return x.first;
        }
    }

    return nullptr;
}

double Server::get_wallet(std::string id) const
{
    for (auto x : clients) {
        if (x.first->get_id() == id) {
            return x.second;
        }
    }

    throw std::logic_error("There is no such id");
}

/*void show_wallets(const Server& server)
{
    std::cout << std::string(20, '*') << std::endl;
    for (const auto& client : server.clients)
        std::cout << client.first->get_id() << " : " << client.second << std::endl;
    std::cout << std::string(20, '*') << std::endl;
}*/

bool Server::parse_trx(std::string trx, std::string sender, std::string receiver, double value)
{
    std::string T{};
    std::stringstream X(trx);
    while (std::getline(X, T, '-')) {
        std::cout << T << std::endl;
    }
    return 0;
}
