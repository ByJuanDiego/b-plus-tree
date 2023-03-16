#include <iostream>
#include <fstream>
#include <utility>
#include "bplustree.h"

struct transaction {
    std::string emisor;
    std::string receptor;
    int ammount;
    int date; // unix timestamp date

    explicit transaction(std::string e, std::string r, int date, int ammount) :
            emisor(std::move(e)),
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
    int M = 3;
    std::function<int(transaction *)> func = [&](transaction *tx) -> int { return tx->ammount; };
    BPlusTree<int, transaction *> bp(func, M);

    std::ifstream file("transactions.txt");
    std::string emisor, receptor;
    int date, ammount;

    while (file >> emisor >> receptor >> ammount >> date) {
        auto *t = new transaction(emisor, receptor, date, ammount);
        bp.insert(t);
    }

    for (const transaction *i: bp.search(2200)) {
        std::cout << *i << std::endl;
    } std::cout << std::endl;

    for (const transaction *i: bp.searchMin()) {
        std::cout << *i << std::endl;
    } std::cout << std::endl;

    for (const transaction *i: bp.searchMax()) {
        std::cout << *i << std::endl;
    } std::cout << std::endl;

    for (const transaction *i: bp.searchBelow(50, true)) {
        std::cout << *i << std::endl;
    } std::cout << std::endl;

    for (const transaction *i: bp.searchAbove(6400, false)) {
        std::cout << *i << std::endl;
    } std::cout << std::endl;

    for (const transaction *i: bp.searchBetween(9, 98, false, false)) {
        std::cout << *i << std::endl;
    } std::cout << std::endl;

    std::ofstream resultantTree("out.txt");
    bp.print(resultantTree);
    return 0;
}
