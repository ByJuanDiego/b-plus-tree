//
// Created by juandiego on 3/22/23.
//

#ifndef BPLUS_TREE_TRANSACTION_HPP
#define BPLUS_TREE_TRANSACTION_HPP

#include <ostream>
#include <sstream>

/// Storage for transactional data
struct transaction {

    std::string emisor;             //< The transaction emisor
    std::string receptor;           //< The receiver
    int amount;                     //< The transferred amount

    /// Operator overload << to display a transaction
    friend std::ostream &operator<<(std::ostream &os, const transaction &tx);

    /// Converts the struct to a human-readable format
    [[nodiscard]] std::string to_string() const;

    /// Default constructor
    transaction();

    /// Parametrized constructor
    explicit transaction(std::string emisor, std::string receptor, int amount);

    /// Copy constructor
    [[maybe_unused]] transaction(const transaction &tx);

    /// Move constructor
    transaction(const transaction &&tx) noexcept;

    /// Copy assignment operator
    transaction &operator=(const transaction &tx);

    /// Move assignment operator
    transaction &operator=(transaction &&tx) noexcept;

    /// Destructor
    ~transaction();

};

#endif //BPLUS_TREE_TRANSACTION_HPP
