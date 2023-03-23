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

transaction::transaction(std::string emisor, std::string receptor, int amount) :
        emisor(std::move(emisor)),
        receptor(std::move(receptor)),
        amount(amount) {
}

//-----------------------------------------------------------------------------

transaction::~transaction() = default;

//-----------------------------------------------------------------------------

std::ostream &operator<<(std::ostream &os, const transaction &tx) {
    os << "(" << tx.emisor << "," << tx.receptor << "," << tx.amount << ")";
    return os;
}

//-----------------------------------------------------------------------------
