#include "client.h"

Client::Client(std::string _id, const Server& _server) // Define Constructor
    : id { _id }
    , server { &_server }
{
    crypto::generate_key(public_key, private_key); // generate public and private key
}

std::string Client::get_id() // get id
{
    return id;
}

double Client::get_wallet() // get wallet
{
    return server->get_wallet(id);
}

std::string Client::get_publickey() const // find public key
{
    // std::string public_key {}, private_key {};
    // crypto::generate_key(public_key, private_key);
    return public_key;
}

std::string Client::sign(std::string txt) const // sign the txt using private key
{
    // std::string public_key {}, private_key {};
    // crypto::generate_key(public_key, private_key);
    std::string signature { crypto::signMessage(private_key, txt) };
    return signature;
}

bool Client::transfer_money(std::string receiver, double value) // make transaction and validate it using
// add_pending_trx
{
    std::string trx { id + "-" + receiver + "-" + std::to_string(value) };
    bool valid_or_invalid { server->add_pending_trx(trx, Client::sign(trx)) };
    return valid_or_invalid;
}

size_t Client::generate_nonce() // generate an integer between 1 to 999999 called nonce
{
    std::random_device random_device; // define random generator
    std::mt19937 engine { random_device() };
    std::uniform_real_distribution<double> dist(1, 999999.99);
    return static_cast<size_t>(dist(engine)); // find its integer and positive part and cast it to size_t
}