#include <iostream>
#include <fstream>
#include <utility>
#include <sstream>
#include "bplustree.h"

struct transaction {
    std::string emisor;
    std::string receptor;
    int amount;
    int date; // unix timestamp date

    explicit transaction(std::string e, std::string r, int date, int ammount) :
            emisor(std::move(e)),
            receptor(std::move(r)), date(date),
            amount(ammount) {
    }

    [[nodiscard]] std::string to_string() const {
        std::stringstream ss;
        ss << *this;
        return ss.str();
    }

    friend std::ostream &operator<<(std::ostream &os, const transaction &tx);
};

std::ostream &operator<<(std::ostream &os, const transaction &tx) {
    os << "(" << tx.emisor << "," << tx.receptor << "," << tx.amount << "," << tx.date << ")";
    return os;
}

int main() {
    int M = 3;
    std::function<int(transaction *)> index = [&](transaction *tx) -> int { return tx->amount; };
    std::function<bool(int, int)> greater = [&](int a, int b)->bool {return a>b;};
    b_plus_tree<int, transaction *, decltype(greater)> bp(index, M, greater);

    std::ifstream file("transactions.txt");
    std::string emisor, receptor;
    int date, ammount;

    while (file >> emisor >> receptor >> ammount >> date) {
        auto *t = new transaction(emisor, receptor, date, ammount);
        bp.insert(t);
    }

    for (const transaction *i: bp.search(2200)) {
        std::cout << i->to_string() << std::endl;
    }
    std::cout << std::endl;

    for (const transaction *i: bp.search_min()) {
        std::cout << i->to_string() << std::endl;
    }
    std::cout << std::endl;

    for (const transaction *i: bp.search_max()) {
        std::cout << i->to_string() << std::endl;
    }
    std::cout << std::endl;

    for (const transaction *i: bp.search_below(50, false)) {
        std::cout << *i << std::endl;
    }
    std::cout << std::endl;

    for (const transaction *i: bp.search_above(6400, false)) {
        std::cout << i->to_string() << std::endl;
    }
    std::cout << std::endl;

    for (const transaction *i: bp.search_between(10, 97, true, false)) {
        std::cout << i->to_string() << std::endl;
    }
    std::cout << std::endl;

    std::ofstream resultantTree("out.txt");
    bp.print(resultantTree, [](std::ostream& os, transaction* tx){
        os << *tx;
    });
    return 0;
}
