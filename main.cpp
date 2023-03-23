#include <iostream>
#include <fstream>
#include <iomanip>

#include "src/bplustree.cpp"
#include "utils/include/transaction.hpp"

int main() {
    std::function<int(transaction *)> index = [&](transaction *tx) -> int { return tx->amount; };
    std::function<bool(int, int)> greater = [&](int a, int b) -> bool { return a > b; };
    b_plus_tree<4, int, transaction *, std::function<bool(int, int)>> bPlusTree(index, greater);
    std::list<transaction *> destructor;

    std::ifstream file("./utils/assets/transactions.txt");
    std::string emisor, receptor;
    int amount;

    int j = 1;
    while (file >> emisor >> receptor >> amount) {
        auto* tx = new transaction(emisor, receptor, amount);
        bPlusTree.insert(tx);
        destructor.push_back(tx);

        std::ofstream resultantTree("./utils/out/transactions_tree_step_" + std::to_string(j++) + ".txt");
        bPlusTree.print(resultantTree, [](std::ostream &os, transaction *tx) {
            os << std::quoted(tx->to_string());
        });
        resultantTree.close();
    }

    std::cout << std::boolalpha << bPlusTree.find(2200) << std::endl << std::endl;

    for (const transaction *tx: bPlusTree.search(2200)) {
        std::cout << tx->to_string() << std::endl;
    }
    std::cout << std::endl;

    for (const transaction *tx: bPlusTree.search_min()) {
        std::cout << tx->to_string() << std::endl;
    }
    std::cout << std::endl;

    for (const transaction *tx: bPlusTree.search_max()) {
        std::cout << tx->to_string() << std::endl;
    }
    std::cout << std::endl;

    for (const transaction *tx: bPlusTree.search_below(50, false)) {
        std::cout << *tx << std::endl;
    }
    std::cout << std::endl;

    for (const transaction *tx: bPlusTree.search_above(1000, false)) {
        std::cout << tx->to_string() << std::endl;
    }
    std::cout << std::endl;

    for (const transaction *tx: bPlusTree.search_between(10, 97, true, false)) {
        std::cout << tx->to_string() << std::endl;
    }
    std::cout << std::endl;
    
    for (transaction* tx : destructor) {
        delete tx;
    }
    return 0;
}
