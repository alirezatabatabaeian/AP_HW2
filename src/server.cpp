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

bool Server::parse_trx(std::string trx, std::string& sender, std::string& receiver, double& value)
{
    std::string T;
    std::stringstream X(trx);
    size_t i {};
    while (getline(X, T, '-')) {
        i++;
    }

    if (i == 3) {
        std::string T;
        std::stringstream X(trx);
        for (size_t i {}; i < 3; i++) {
            std::getline(X, T, '-');
            if (i == 0) {
                sender = T;
            } else if (i == 1) {
                receiver = T;
            } else {
                value = std::stod(T);
            }
        }
        return 1;
    } else {
        throw std::runtime_error("Runtime error occurred");
    }
}

bool Server::add_pending_trx(std::string trx, std::string signature) const
{
    std::string sender {}, receiver {};
    double value {};
    Server::parse_trx(trx, sender, receiver, value);

    std::shared_ptr<Client> sender_ptr { Server::get_client(sender) };
    std::shared_ptr<Client> receiver_ptr { Server::get_client(receiver) };

    if (sender_ptr == nullptr) {
        return 0;
    } else if (receiver_ptr == nullptr) {
        return 0;
    } else if (sender_ptr->get_wallet() < value) {
        return 0;
    } else if (crypto::verifySignature(sender_ptr->get_publickey(), trx, signature) == 0) {
        return 0;
    }

    pending_trxs.push_back(trx);

    return 1;
}