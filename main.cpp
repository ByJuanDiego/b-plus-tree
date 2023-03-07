#include <iostream>
#include "bplustree.h"

int main() {
    BPlusTree<int> bPlusTree(4);

    for (int i=1; i<=10; ++i) {
        bPlusTree.insert(i);
    }
    bPlusTree.insert(9);

    bPlusTree.print(std::cout);
    std::cout << std::endl;

    for (int i : bPlusTree.rangeSearch(6, 9)){
        std::cout << i << " ";
    }
    return 0;
}
