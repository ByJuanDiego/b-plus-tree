//
// Created by Juan Diego on 3/7/2023.
//

#ifndef BPLUS_TREE_NODE_H
#define BPLUS_TREE_NODE_H

#include <vector>

template<typename KT>
struct Node {
    std::vector<KT> keys;
    std::vector<Node<KT> *> children;
    Node<KT>* next_leaf;
    bool leaf;
    int count;

    explicit Node(int M, bool is_leaf = false) : count(0), leaf(is_leaf), next_leaf(nullptr) {
        keys.resize(M, 0);
        children.resize(M + 1, nullptr);
    }

    ~Node() {
        keys.clear();
        children.clear();
    }

    void killSelf() {
        for (int i = 0; i <= count; ++i) {
            if (children[i]) {
                children[i]->killSelf();
            }
        }
        delete this;
    }

    void print(std::ostream &os, const int &M) const {
        os << "[";
        for (int i = 0; i < (M - 1); ++i) {
            os << ((i < count) ? std::to_string(keys[i]) : "") << ((i < M - 2) ? "|" : "");
        }
        os << "]";
    }
};

#endif //BPLUS_TREE_NODE_H
