#include <iostream>
#include <fstream>
#include <random>
#include <memory>

#include "bplustree.cpp"
#include "transaction.hpp"

void insertion_test(auto& tree) {
    std::ifstream transactionsFile("./utils/database/transactions.txt");
    std::string emisor, receptor;
    int amount;

    std::ofstream insertTree("./logs/insert.txt");
    while (transactionsFile >> emisor >> receptor >> amount) {
        insertTree << "to insert: " << amount << std::endl;

        std::shared_ptr<transaction> tx = std::make_shared<transaction>(emisor, receptor, amount);
        tree.insert(tx);
        tree.print(insertTree, [](std::ostream &os, const std::shared_ptr<transaction>& tx) {
            os << "";
        });
        insertTree << "\n\n";
    }
    insertTree.close();
}

void search_test(auto& tree) {
    int key {123};
    if (tree.find(key)) {
        for (const std::shared_ptr<transaction>& tx: tree.search(key)) {
            std::cout << tx->to_string() << std::endl;
        }
        std::cout << std::endl;
    }

    for (const std::shared_ptr<transaction>& tx: tree.search_min()) {
        std::cout << tx->to_string() << std::endl;
    }
    std::cout << std::endl;

    for (const std::shared_ptr<transaction>& tx: tree.search_max()) {
        std::cout << tx->to_string() << std::endl;
    }
    std::cout << std::endl;

    for (const std::shared_ptr<transaction>& tx: tree.search_below(45, false)) {
        std::cout << tx->to_string() << std::endl;
    }
    std::cout << std::endl;

    for (const std::shared_ptr<transaction>& tx: tree.search_above(471, false)) {
        std::cout << tx->to_string() << std::endl;
    }
    std::cout << std::endl;

    int min{10}, max{97};
    bool include_min{true}, include_max{false};
    for (const std::shared_ptr<transaction>& tx: tree.search_between(min, max, include_min, include_max)) {
        std::cout << tx->to_string() << std::endl;
    }
    std::cout << std::endl;
}

void delete_test(auto& bPlusTree) {
    std::uniform_int_distribution<int> dis(1, 499);
    std::random_device rd;

    std::ofstream removeTree("./logs/remove.txt");
    while (!bPlusTree.empty()) {
        int key = dis(rd);
        if (bPlusTree.find(key)) {
            removeTree << "to delete: " << key << std::endl;
            bPlusTree.remove(key);
            bPlusTree.print(removeTree,[](std::ostream &os, const std::shared_ptr<transaction>& tx) {
                os << "";
            });
            removeTree << "\n\n";
        }
    }
    removeTree.close();
}

int main() {
    auto index = [&](const std::shared_ptr<transaction>& tx) -> int { return tx->amount; };
    auto greater = [&](int a, int b) -> bool { return a > b; };
    b_plus_tree<7, int, std::shared_ptr<transaction>, decltype(greater), decltype(index)> bPlusTree(index, greater);
    insertion_test(bPlusTree);
    search_test(bPlusTree);
    delete_test(bPlusTree);

    return EXIT_SUCCESS;
}
