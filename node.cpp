//
// Created by juandiego on 3/19/23.
//

#include "node.hpp"

template<typename K>
node<K>::node(int degree, bool is_leaf) : num_keys(0), is_leaf(is_leaf) {
    keys.resize(degree, K());
    children.resize(degree + 1, nullptr);
}

template<typename K>
node<K>::~node() {
    keys.clear();
    children.clear();
}

template<typename K>
void node<K>::kill_self() {
    for (int i = 0; i <= this->num_keys; ++i) {
        if (this->children[i]) {
            this->children[i]->kill_self();
        }
    }
    delete this;
}

template<typename K>
void internal_node<K>::split(node<K> *&father, int i, int degree, int m) {
    auto *partition = new internal_node<K>(degree, false);
    partition->num_keys = m;

    for (int j = 0; j < m; ++j)
        partition->keys[j] = this->keys[j + m + 1 + ((degree % 2) ? 0 : 1)];

    for (int j = 0; j < m + 1; ++j)
        partition->children[j] = this->children[j + m + 1 + ((degree % 2) ? 0 : 1)];

    for (int j = father->num_keys; j > i; --j)
        father->children[j + 1] = father->children[j];

    for (int j = father->num_keys; j > i; --j)
        father->keys[j] = father->keys[j - 1];

    this->num_keys = m + ((degree % 2) ? 0 : 1);

    father->children[i + 1] = partition;
    father->keys[i] = this->keys[m + ((degree % 2) ? 0 : 1)];
    father->num_keys++;
}

template<typename K>
void internal_node<K>::print(std::ostream &os, PrintFunction<K> disp_k) {
    os << "[";
    for (int i = 0; i < this->num_keys; ++i) {
        disp_k(os, this->keys[i]);
        os << ((i < (this->num_keys - 1)) ? "|" : "");
    }
    os << "]";
}

template<typename K, typename V>
leaf_node<K, V>::leaf_node(int degree, bool is_leaf) :
        node<K>(degree, is_leaf),
        next_leaf(nullptr),
        prev_leaf(nullptr) {
    records.resize(degree, V());
}

template<typename K, typename V>
leaf_node<K, V>::~leaf_node() {
    records.clear();
}

template<typename K, typename V>
void leaf_node<K, V>::split(node<K> *&father, int i, int degree, int m) {
    auto *partition = new leaf_node<K, V>(degree, true);
    partition->num_keys = m + ((degree % 2) ? 0 : 1);

    partition->next_leaf = next_leaf;
    partition->prev_leaf = this;
    if (next_leaf) {
        next_leaf->prev_leaf = partition;
    }
    next_leaf = partition;

    for (int j = 0; j < m + ((degree % 2) ? 0 : 1); ++j) {
        partition->keys[j] = this->keys[j + m + 1];
        partition->records[j] = this->records[j + m + 1];
    }

    for (int j = father->num_keys; j > i; --j)
        father->children[j + 1] = father->children[j];

    for (int j = father->num_keys; j > i; --j)
        father->keys[j] = father->keys[j - 1];

    this->num_keys = m + 1;

    father->children[i + 1] = partition;
    father->keys[i] = this->keys[m];
    father->num_keys++;
}

template<typename K, typename V>
void leaf_node<K, V>::print(std::ostream &os, PrintFunction<K> disp_k, PrintFunction<V> disp_v) {
    os << "[";
    for (int i = 0; i < this->num_keys; ++i) {
        os << "(";
        disp_k(os, this->keys[i]);
        os << ":";
        disp_v(os, this->records[i]);
        os << ")";
        os << ((i < (this->num_keys - 1)) ? "|" : "");
    }
    os << "]";
}
