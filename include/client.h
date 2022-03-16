#ifndef CLIENT_H
#define CLIENT_H

#include "crypto.h"
#include "server.h"
#include <cstring>
#include <iostream>
#include <map>
#include <memory>
#include <random>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

class Server;

class Client {
public:
    Client(std::string id, const Server& server);
    std::string get_id();
    std::string get_publickey() const;
    double get_wallet();
    std::string sign(std::string txt) const;
    bool transfer_money(std::string receiver, double value);
    size_t generate_nonce();

private:
    Server const* const server;
    const std::string id;
    std::string public_key;
    std::string private_key;
};

static void show_wallets(const Server& server)
{
    std::cout << std::string(20, '*') << std::endl;
    std::map<std::shared_ptr<Client>, double>* temp { (std::map<std::shared_ptr<Client>, double>*)&server };
    for (auto i { temp->begin() }; i != temp->end(); i++)
        std::cout << i->first->get_id() << " : " << i->second << std::endl;
    std::cout << std::string(20, '*') << std::endl;
}

#endif // CLIENT_H