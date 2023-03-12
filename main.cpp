#include <iostream>
#include <fstream>
#include <utility>
#include "bplustree.h"

struct transaction {
    std::string emisor;
    std::string receptor;
    int ammount;
    int date;

    explicit transaction(std::string e, std::string r, int date, int ammount) : emisor(std::move(e)),
                                                                                receptor(std::move(r)), date(date),
                                                                                ammount(ammount) {
    }

    friend std::ostream &operator<<(std::ostream &os, const transaction &transaction);
};

std::ostream &operator<<(std::ostream &os, const transaction &transaction) {
    os << "emisor: " << transaction.emisor << ", receptor: " << transaction.receptor << ", ammount: "
       << transaction.ammount << ", date: " << transaction.date;
    return os;
}

int main() {
    BPlusTree<transaction *, int> bp([&](transaction *tx) {
        return tx->ammount;
    }, 5);

    std::ifstream file("transactions.txt");
    std::string emisor, receptor;
    int date, ammount;

    while (file >> emisor >> receptor >> ammount >> date) {
        auto *t = new transaction(emisor, receptor, date, ammount);
        bp.insert(t);
    }

    for (const transaction *i: bp.searchBetween(500, 900)) {
        std::cout << *i << std::endl;
    }

    return 0;
}
