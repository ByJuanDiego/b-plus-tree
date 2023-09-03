#include <iostream>
#include <fstream>
#include <iomanip>

#include "bplustree.cpp"
#include "transaction.hpp"

int main() {
    auto index = [&](const transaction *tx) -> int { return tx->amount; };
    auto greater = [&](int a, int b) -> bool { return a > b; };
    b_plus_tree<4, int, transaction *, decltype(greater), decltype(index)> bPlusTree(index, greater);
    std::list<transaction *> destructor;

    std::ifstream file("./utils/assets/transactions.txt");
    std::string emisor, receptor;
    int amount;

    std::ofstream insertTree("./utils/out/insert.txt");
    while (file >> emisor >> receptor >> amount) {
        auto *tx = new transaction(emisor, receptor, amount);
        bPlusTree.insert(tx);
        destructor.push_back(tx);

        bPlusTree.print(insertTree, [](std::ostream &os, const transaction *tx) {
            os << "";
        });
        insertTree << "\n\n";
    }
    insertTree.close();

    bPlusTree.print(std::cout, [](std::ostream &os, const transaction *tx) {
            os << std::quoted(tx->to_string());
    });


    int key{2200};
    if (bPlusTree.find(key)) {
        for (const transaction *tx: bPlusTree.search(key)) {
            std::cout << tx->to_string() << std::endl;
        }
        std::cout << std::endl;
    }

    for (const transaction *tx: bPlusTree.search_min()) {
        std::cout << tx->to_string() << std::endl;
    }
    std::cout << std::endl;

    for (const transaction *tx: bPlusTree.search_max()) {
        std::cout << tx->to_string() << std::endl;
    }
    std::cout << std::endl;

    for (const transaction *tx: bPlusTree.search_below(50, false)) {
        std::cout << tx->to_string() << std::endl;
    }
    std::cout << std::endl;

    for (const transaction *tx: bPlusTree.search_above(1000, false)) {
        std::cout << tx->to_string() << std::endl;
    }
    std::cout << std::endl;

    int min{10}, max{97};
    bool include_min{true}, include_max{false};
    for (const transaction *tx: bPlusTree.search_between(min, max, include_min, include_max)) {
        std::cout << tx->to_string() << std::endl;
    }
    std::cout << std::endl;

    std::ofstream removeTree("./utils/out/remove.txt");
    for (transaction* tx: destructor) {
        bPlusTree.remove(tx->amount);
        bPlusTree.print(removeTree, [](std::ostream &os, const transaction *tx) {
            os << "";
        });
        removeTree << "\n\n";
    }
    removeTree.close();

    for (transaction *tx: destructor) {
        delete tx;
    }
    return 0;
}
