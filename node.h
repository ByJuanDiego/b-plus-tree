//
// Created by Juan Diego on 3/7/2023.
//

#ifndef BPLUS_TREE_NODE_H
#define BPLUS_TREE_NODE_H

#include <vector>
#include <string>
#include <ostream>

template<typename K>
struct node {
    std::vector<K> keys;
    int n_keys;

    std::vector<node<K> *> children;
    bool is_leaf;

    explicit node(int M, bool is_leaf = false) : n_keys(0), is_leaf(is_leaf) {
        keys.resize(M, K());
        children.resize(M + 1, nullptr);
    }

    virtual ~node() {
        keys.clear();
        children.clear();
    }

    virtual void kill_self() {
        for (int i = 0; i <= this->n_keys; ++i) {
            if (this->children[i]) {
                this->children[i]->kill_self();
            }
        }
        delete this;
    }

    virtual void split(node<K> *&father, int i, int M, int m) = 0;
};

template<typename K>
struct internal_node : public node<K> {

    using node<K>::node;

    virtual ~internal_node() = default;

    void print(std::ostream &os, std::function<void(std::ostream &, K)> print_key) {
        os << "[";
        for (int i = 0; i < this->n_keys; ++i) {
            print_key(os, this->keys[i]);
        }
        os << "]";
    }

    void split(node<K> *&father, int i, int M, int m) override {
        auto *partition = new internal_node<K>(M, false);
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

template<typename K, typename V>
struct leaf_node : public node<K> {
    std::vector<V> records;
    leaf_node<K, V> *next_leaf;
    leaf_node<K, V> *prev_leaf;

    explicit leaf_node(int M, bool is_leaf = true) : node<K>(M, is_leaf), next_leaf(nullptr), prev_leaf(nullptr) {
        records.resize(M, V());
    }

    virtual ~leaf_node() {
        records.clear();
    }

    void print(std::ostream &os,
               std::function<void(std::ostream &, K)> key_print,
               std::function<void(std::ostream &, V)> value_print) {
        os << "[";
        for (int i = 0; i < this->n_keys; ++i) {
            key_print(os, this->keys[i]);
            os << ":";
            value_print(os, this->records[i]);
        }
        os << "]";
    }

    void split(node<K> *&father, int i, int M, int m) override {
        auto *partition = new leaf_node<K, V>(M, true);
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
