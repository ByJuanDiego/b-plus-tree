#include <iostream>
#include <fstream>
#include <utility>
#include <iomanip>

#include "bplustree.cpp"

struct transaction {
    std::string emisor;
    std::string receptor;
    int amount;

    explicit transaction(std::string e, std::string r, int date, int amount) :
            emisor(std::move(e)),
            receptor(std::move(r)),
            amount(amount) {
    }

    [[nodiscard]] std::string to_string() const {
        std::stringstream ss;
        ss << *this;
        return ss.str();
    }

    friend std::ostream &operator<<(std::ostream &os, const transaction &tx) {
        os << "(" << tx.emisor << "," << tx.receptor << "," << tx.amount << ")";
        return os;
    }
};

int main() {
    std::function<int(transaction *)> index = [&](transaction *tx) -> int { return tx->amount; };
    std::function<bool(int, int)> greater = [&](int a, int b) -> bool { return a > b; };
    b_plus_tree<5, int, transaction *, std::function<bool(int, int)>> bPlusTree(index, greater);

    std::ifstream file("transactions.txt");
    std::string emisor, receptor;
    int date, amount;

    while (file >> emisor >> receptor >> amount >> date) {
        bPlusTree.insert(new transaction(emisor, receptor, date, amount));
    }

    for (const transaction *i: bPlusTree.search(2200)) {
        std::cout << i->to_string() << std::endl;
    }
    std::cout << std::endl;

    for (const transaction *i: bPlusTree.search_min()) {
        std::cout << i->to_string() << std::endl;
    }
    std::cout << std::endl;

    for (const transaction *i: bPlusTree.search_max()) {
        std::cout << i->to_string() << std::endl;
    }
    std::cout << std::endl;

    for (const transaction *i: bPlusTree.search_below(50, false)) {
        std::cout << *i << std::endl;
    }
    std::cout << std::endl;

    for (const transaction *i: bPlusTree.search_above(1000, false)) {
        std::cout << i->to_string() << std::endl;
    }
    std::cout << std::endl;

    for (const transaction *i: bPlusTree.search_between(10, 97, true, false)) {
        std::cout << i->to_string() << std::endl;
    }
    std::cout << std::endl;

    std::ofstream resultantTree("out.txt");
    bPlusTree.print(resultantTree, [](std::ostream &os, transaction *tx) {
        os << std::quoted(tx->to_string());
    });

    bPlusTree.clear();
    resultantTree.close();
    return 0;
}
