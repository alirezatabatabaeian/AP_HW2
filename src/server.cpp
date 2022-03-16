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

size_t Server::mine()
{
    std::string all_trxs {};
    size_t nonce {};
    std::string all_trxs_nonce {};
    bool if_mined {};

    for (auto one_trx : pending_trxs) {
        all_trxs += one_trx;
    }

    while (if_mined == 0) {
        for (auto miner : clients) { //
            // for (auto trx : pending_trxs) {
            //  std::string sender {};
            //  std::string receiver {};
            //  double value {};
            //  Server::parse_trx(trx, sender, receiver, value);
            //  std::shared_ptr<Client> miner { get_client(sender) };
            //  nonce = miner->generate_nonce();
            nonce = miner.first->generate_nonce(); //
            all_trxs_nonce = all_trxs + std::to_string(nonce);
            std::string hash { crypto::sha256(all_trxs_nonce) };
            if (hash.substr(0, 10).find("000") != std::string::npos) {
                if_mined = 1;
                // clients[miner] += 6.25;
                clients[miner.first] += 6.25; //
                break;
            }
        }
    }

    for (auto trx : pending_trxs) {
        std::string sender;
        std::string receiver;
        double value;
        parse_trx(trx, sender, receiver, value);
        std::shared_ptr<Client> sender_ptr { get_client(sender) };
        std::shared_ptr<Client> receiver_ptr { get_client(receiver) };
        clients[sender_ptr] -= value;
        clients[receiver_ptr] += value;
    }

    pending_trxs = {};
    return nonce;
}