#include "server.h"

Server::Server() { } // Define Constructor

std::shared_ptr<Client> Server::add_client(std::string _id) // add client using its id
{
    for (auto x : clients) {
        if (x.first->get_id() == _id) {
            for (size_t i {}; i < 4; i++) {
                std::random_device random_device; // define random generator
                std::mt19937 engine { random_device() };
                std::uniform_real_distribution<double> dist(0, 9.99);
                _id = _id + std::to_string(static_cast<int>(dist(engine))); // add random number to id if
            } // the id is repeated
        }
    }
    Client temp { _id, *this }; // make a temporary id
    std::shared_ptr<Client> temp2 { std::make_shared<Client>(temp) }; // make a shared pointer to the previous id
    clients.insert(std::pair<std::shared_ptr<Client>, double>(temp2, 5.0)); // insert user with its initial deposit
    return temp2;
}

std::shared_ptr<Client> Server::get_client(std::string id) const // get client using its id
{
    for (auto x : clients) { // iterate on pre-defined clients
        if (x.first->get_id() == id) { // compare id with input id
            return x.first; // return id shared pointer
        }
    }

    return nullptr; // return nullptr if the id doesn't exist
}

double Server::get_wallet(std::string id) const // get client wallet using its id
{
    for (auto x : clients) { // iterate on pre-defined clients
        if (x.first->get_id() == id) { // compare id with input id
            return x.second; // return users money
        }
    }

    throw std::logic_error("There is no such id"); // return logical error if the id doesn't exist
}

bool Server::parse_trx(std::string trx, std::string& sender, std::string& receiver, double& value)
{ // parse each input transaction into three variables called : sender, receiver, value
    // last three inputs must be in refrence form
    std::string T;
    std::stringstream X(trx);
    size_t i {};
    while (getline(X, T, '-')) { // de-concate an string using dash sign
        i++;
    }

    if (i == 3) { // if trx is correct
        std::string T;
        std::stringstream X(trx);
        for (size_t i {}; i < 3; i++) { // copy sender and receiver and value to variables
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
    } else { // if trx is wrong
        throw std::runtime_error("Runtime error occurred"); // throw runtime error
    }
}

bool Server::add_pending_trx(std::string trx, std::string signature) const
{ // add pending transaction
    std::string sender {}, receiver {};
    double value {};
    Server::parse_trx(trx, sender, receiver, value); // parse transactions

    std::shared_ptr<Client> sender_ptr { Server::get_client(sender) }; // make pointer to sender
    std::shared_ptr<Client> receiver_ptr { Server::get_client(receiver) }; // make pointer to receiver

    if (sender_ptr == nullptr) { // return 0 if there is no sender
        return 0;
    } else if (receiver_ptr == nullptr) { // return 0 if there is no receiver
        return 0;
    } else if (sender_ptr->get_wallet() < value) { // return 0 if money is not enough
        return 0;
    } else if (crypto::verifySignature(sender_ptr->get_publickey(), trx, signature) == 0) {
        return 0; // return 0 if signature is wrong
    }

    pending_trxs.push_back(trx); // push back transaction to pending_trxs string-vector

    return 1;
}

size_t Server::mine() // mine transactions
{
    std::string all_trxs {};
    size_t nonce {};
    std::string all_trxs_nonce {};
    bool if_mined {};

    for (auto one_trx : pending_trxs) { // concat transactions to each other
        all_trxs += one_trx;
    }

    while (if_mined == 0) {
        for (auto miner : clients) { // iterate on all clients
            // for (auto trx : pending_trxs) {
            //  std::string sender {};
            //  std::string receiver {};
            //  double value {};
            //  Server::parse_trx(trx, sender, receiver, value);
            //  std::shared_ptr<Client> miner { get_client(sender) };
            //  nonce = miner->generate_nonce();
            nonce = miner.first->generate_nonce(); // generate nonce
            all_trxs_nonce = all_trxs + std::to_string(nonce); // concat transactions to nonce
            std::string hash { crypto::sha256(all_trxs_nonce) }; // hash the string using sha256
            if (hash.substr(0, 10).find("000") != std::string::npos) { // find 000 in first 10 numbers
                if_mined = 1; // this variable helps to finish while loop
                // clients[miner] += 6.25;
                clients[miner.first] += 6.25; // reward the true miner
                std::cout << "Miner is : " << miner.first->get_id() << std::endl; // print true miner id
                break;
            }
        }
    }

    for (auto trx : pending_trxs) { // iterate on pending transactions
        std::string sender;
        std::string receiver;
        double value;
        parse_trx(trx, sender, receiver, value); // parse transactions
        std::shared_ptr<Client> sender_ptr { get_client(sender) }; // make pointer to sender
        std::shared_ptr<Client> receiver_ptr { get_client(receiver) }; // make pointer to receiver
        clients[sender_ptr] -= value; // subtract transfered value from sender
        clients[receiver_ptr] += value; // plus transfered value to receiver
    }

    pending_trxs = {}; // empty pending_trxs
    return nonce;
}