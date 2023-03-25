//
// Created by juandiego on 3/22/23.
//

#include "../include/transaction.hpp"

//-----------------------------------------------------------------------------

std::string transaction::to_string() const {
    std::stringstream ss;
    ss << *this;
    return ss.str();
}

//-----------------------------------------------------------------------------

transaction::transaction() : amount(0) {
}

//-----------------------------------------------------------------------------

transaction::transaction(std::string emisor, std::string receptor, int amount) :
        emisor(std::move(emisor)),
        receptor(std::move(receptor)),
        amount(amount) {
}

//-----------------------------------------------------------------------------

transaction::transaction(const transaction &tx) = default;

//-----------------------------------------------------------------------------

transaction::transaction(const transaction &&tx) noexcept {
    this->emisor = tx.emisor;
    this->receptor = tx.receptor;
    this->amount = tx.amount;
}

//-----------------------------------------------------------------------------

transaction &transaction::operator=(const transaction &tx) {
    if (&tx == this) {
        return *this;
    }
    this->emisor = tx.emisor;
    this->receptor = tx.receptor;
    this->amount = tx.amount;
    return *this;
}

//-----------------------------------------------------------------------------

transaction &transaction::operator=(transaction &&tx) noexcept {
    this->emisor = tx.emisor;
    this->receptor = tx.receptor;
    this->amount = tx.amount;
    return *this;
}

//-----------------------------------------------------------------------------

transaction::~transaction() = default;

//-----------------------------------------------------------------------------

std::ostream &operator<<(std::ostream &os, const transaction &tx) {
    os << "(" << tx.emisor << "," << tx.receptor << "," << tx.amount << ")";
    return os;
}

//-----------------------------------------------------------------------------
