//
// Created by juandiego on 3/22/23.
//

#ifndef BPLUS_TREE_TRANSACTION_HPP
#define BPLUS_TREE_TRANSACTION_HPP

#include <ostream>
#include <sstream>

struct transaction {

    std::string emisor;
    std::string receptor;
    int amount;

    friend std::ostream &operator<<(std::ostream &os, const transaction &tx);

    explicit transaction(std::string emisor, std::string receptor, int amount);

    ~transaction();

    [[nodiscard]] std::string to_string() const;

};

#endif //BPLUS_TREE_TRANSACTION_HPP
