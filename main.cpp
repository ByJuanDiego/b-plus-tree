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
    os << "e: " << transaction.emisor << " r: " << transaction.receptor << " a: " << transaction.ammount << " d: "
       << transaction.date;
    return os;
}

int main() {
    BPlusTree<int, transaction *> bp(5);
    std::ifstream file("transactions.txt");
    std::string emisor;
    std::string receptor;
    int date;
    int ammount;

    while (file >> emisor >> receptor >> ammount >> date) {
        auto *t = new transaction(emisor, receptor, date, ammount);
        bp.insert(t->ammount, t);
    }

    for (const transaction *i: bp.searchBetween(15, 900)) {
        std::cout << *i << std::endl;
    }
    std::cout << std::endl;
    return 0;
}
