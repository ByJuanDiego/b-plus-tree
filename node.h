//
// Created by Juan Diego on 3/7/2023.
//

#ifndef BPLUS_TREE_NODE_H
#define BPLUS_TREE_NODE_H

#include <vector>
#include <string>
#include <ostream>

template<typename KT>
struct Node {
    std::vector<KT> keys;
    int n_keys;

    std::vector<Node<KT> *> children;
    bool is_leaf;

    explicit Node(int M, bool is_leaf = false) : n_keys(0), is_leaf(is_leaf) {
        keys.resize(M, KT());
        children.resize(M + 1, nullptr);
    }

    virtual ~Node() {
        keys.clear();
        children.clear();
    }

    virtual void killSelf() {
        for (int i = 0; i <= this->n_keys; ++i) {
            if (this->children[i]) {
                this->children[i]->killSelf();
            }
        }
        delete this;
    }

    virtual void print(std::ostream &os, int M) = 0;

    virtual void split(Node<KT> *&father, int i, int M, int m) = 0;
};

template<typename KT>
struct internalNode : public Node<KT> {

    using Node<KT>::Node;

    virtual ~internalNode() = default;

    void print(std::ostream &os, int M) override {
        os << "[";
        for (int i = 0; i < M; ++i) {
            os << (i < this->n_keys ? std::to_string(this->keys[i]) : "") << ((i < (M - 2)) ? "|" : "");
        }
        os << "]";
    }

    void split(Node<KT> *&father, int i, int M, int m) override {
        auto *partition = new internalNode<KT>(M, false);
        partition->n_keys = m;

        for (int j = 0; j < m; ++j)
            partition->keys[j] = this->keys[j + m + 1 + ((M % 2) ? 0 : 1)];

        for (int j = 0; j < m + 1; ++j)
            partition->children[j] = this->children[j + m + 1 + ((M % 2) ? 0 : 1)];

        for (int j = father->n_keys; j > i; --j)
            father->children[j + 1] = father->children[j];

        for (int j = father->n_keys; j > i; --j)
            father->keys[j] = father->keys[j - 1];

        this->n_keys = m + ((M % 2) ? 0 : 1);

        father->children[i + 1] = partition;
        father->keys[i] = this->keys[m + ((M % 2) ? 0 : 1)];
        father->n_keys++;
    }
};

template<typename KT, typename KV>
struct leafNode : public Node<KT> {
    std::vector<KV> records;
    leafNode<KT, KV> *next_leaf;
    leafNode<KT, KV> *prev_leaf;

    explicit leafNode(int M, bool is_leaf = true) : Node<KT>(M, is_leaf), next_leaf(nullptr), prev_leaf(nullptr) {
        records.resize(M, KV());
    }

    virtual ~leafNode() {
        records.clear();
    }

    void print(std::ostream &os, int M) override {
        os << "[";
        for (int i = 0; i < M; ++i) {
            if (i < this->n_keys) {
                os << (std::to_string(this->keys[i]) + std::string(":")) << records[i];
            }
            if (i < (M-2)) {
                os << "|";
            }
        }
        os << "]";
    }

    void split(Node<KT> *&father, int i, int M, int m) override {
        auto *partition = new leafNode<KT, KV>(M, true);
        partition->n_keys = m + ((M % 2) ? 0 : 1);

        partition->next_leaf = next_leaf;
        partition->prev_leaf = this;
        if (next_leaf) {
            next_leaf->prev_leaf = partition;
        }
        next_leaf = partition;

        for (int j = 0; j < m + ((M % 2) ? 0 : 1); ++j) {
            partition->keys[j] = this->keys[j + m + 1];
            partition->records[j] = this->records[j + m + 1];
        }

        for (int j = father->n_keys; j > i; --j)
            father->children[j + 1] = father->children[j];

        for (int j = father->n_keys; j > i; --j)
            father->keys[j] = father->keys[j - 1];


        this->n_keys = m + 1;

        father->children[i + 1] = partition;
        father->keys[i] = this->keys[m];
        father->n_keys++;
    }
};


#endif //BPLUS_TREE_NODE_H
