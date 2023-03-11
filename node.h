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
    bool is_leaf;
    int n_children;

    explicit Node(int M, bool is_leaf = false) : n_children(0), is_leaf(is_leaf) {
        keys.resize(M, KT());
        children.resize(M + 1, nullptr);
    }

    virtual ~Node() {
        keys.clear();
        children.clear();
    }

    virtual void killSelf() {
        for (int i = 0; i <= this->n_children; ++i) {
            if (this->children[i]) {
                this->children[i]->killSelf();
            }
        }
        delete this;
    }

    void print(std::ostream &os, const int &M) const {
        os << "[";
        for (int i = 0; i < (M - 1); ++i) {
            os << ((i < n_children) ? std::to_string(keys[i]) : "") << ((i < M - 2) ? "|" : "");
        }
        os << "]";
    }

    virtual void split(Node<KT> *&father, int i, int M, int m) = 0;
};

template<typename KT>
struct internalNode : public Node<KT> {

    using Node<KT>::Node;

    virtual ~internalNode() = default;

    void split(Node<KT> *&father, int i, int M, int m) override {
        auto *partition = new internalNode<KT>(M, false);
        partition->n_children = m;

        for (int j = 0; j < m; ++j)
            partition->keys[j] = this->keys[j + m + 1 + ((M % 2) ? 0 : 1)];

        for (int j = 0; j < m + 1; ++j)
            partition->children[j] = this->children[j + m + 1 + ((M % 2) ? 0 : 1)];

        for (int j = father->n_children; j > i; --j)
            father->children[j + 1] = father->children[j];

        for (int j = father->n_children; j > i; --j)
            father->keys[j] = father->keys[j - 1];

        this->n_children = m + ((M % 2) ? 0 : 1);

        father->children[i + 1] = partition;
        father->keys[i] = this->keys[m + ((M % 2) ? 0 : 1)];
        father->n_children++;
    }
};

template<typename KT, typename KV>
struct leafNode : public Node<KT> {
    std::vector<KV> records;
    Node<KT> *next_leaf;

    explicit leafNode(int M, bool is_leaf = true) : Node<KT>(M, is_leaf), next_leaf(nullptr) {
        records.resize(M, KV());
    }

    virtual ~leafNode() {
        records.clear();
    }

    void split(Node<KT> *&father, int i, int M, int m) override {
        auto *partition = new leafNode<KT, KV>(M, true);
        partition->n_children = m + ((M % 2) ? 0 : 1);
        partition->next_leaf = next_leaf;

        for (int j = 0; j < m + ((M % 2) ? 0 : 1); ++j)
            partition->keys[j] = this->keys[j + m + 1];

        for (int j = father->n_children; j > i; --j)
            father->children[j + 1] = father->children[j];

        for (int j = father->n_children; j > i; --j)
            father->keys[j] = father->keys[j - 1];

        next_leaf = partition;
        this->n_children = m + 1;

        father->children[i + 1] = partition;
        father->keys[i] = this->keys[m];
        father->n_children++;
    }
};


#endif //BPLUS_TREE_NODE_H
