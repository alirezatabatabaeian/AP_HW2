#include "client.h"

Client::Client(std::string _id, const Server& _server)
    : id { _id }
    , server { &_server }
{
    crypto::generate_key(public_key, private_key);
}

std::string Client::get_id()
{
    return id;
}

double Client::get_wallet()
{
    return server->get_wallet(id);
}

std::string Client::get_publickey() const
{
    // std::string public_key {}, private_key {};
    // crypto::generate_key(public_key, private_key);
    return public_key;
}

std::string Client::sign(std::string txt) const
{
    // std::string public_key {}, private_key {};
    // crypto::generate_key(public_key, private_key);
    std::string signature { crypto::signMessage(private_key, txt) };
    return signature;
}

bool Client::transfer_money(std::string receiver, double value)
{
    std::string trx { id + "-" + receiver + "-" + std::to_string(value) };
    bool valid_or_invalid { server->add_pending_trx(trx, Client::sign(trx)) };
    return valid_or_invalid;
}