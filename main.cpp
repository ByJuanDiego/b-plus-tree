#include <iostream>
#include "bplustree.h"

int main() {
    BPlusTree<int, float> bp;
    bp.insert(1);
    bp.insert(9);
    bp.insert(5);
    bp.insert(7);
    bp.insert(8);
    bp.insert(3);
    bp.print(std::cout);
    return 0;
}
