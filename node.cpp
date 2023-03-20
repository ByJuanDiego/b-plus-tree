//
// Created by juandiego on 3/19/23.
//

#include "node.hpp"

template<typename K>
node<K>::node(int order, bool is_leaf) : num_keys(0), is_leaf(is_leaf) {
    keys = new K[order]{K()};
    children = new node<K> *[order + 1]{nullptr};
}

template<typename K>
node<K>::~node() {
    delete[] keys;
    delete[] children;
};

template<typename K>
void node<K>::kill_self() {
    if (is_leaf) {
        delete this;
        return;
    }

    for (int i = 0; i <= num_keys; ++i) {
        children[i]->kill_self();
    }
}

template<typename K>
void internal_node<K>::split(node<K> *&father, int i, int order, int m) {
    auto *partition = new internal_node<K>(order, false);
    partition->num_keys = m;

    for (int j = 0; j < m; ++j)
        partition->keys[j] = this->keys[j + m + 1 + ((order % 2) ? 0 : 1)];

    for (int j = 0; j < m + 1; ++j)
        partition->children[j] = this->children[j + m + 1 + ((order % 2) ? 0 : 1)];

    for (int j = father->num_keys; j > i; --j)
        father->children[j + 1] = father->children[j];

    for (int j = father->num_keys; j > i; --j)
        father->keys[j] = father->keys[j - 1];

    this->num_keys = m + ((order % 2) ? 0 : 1);

    father->children[i + 1] = partition;
    father->keys[i] = this->keys[m + ((order % 2) ? 0 : 1)];
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
leaf_node<K, V>::leaf_node(int order, bool is_leaf) :
        node<K>(order, is_leaf),
        next_leaf(nullptr),
        prev_leaf(nullptr) {
    records = new V[order]{V()};
}

template<typename K, typename V>
leaf_node<K, V>::~leaf_node() {
    delete[] records;
}

template<typename K, typename V>
void leaf_node<K, V>::split(node<K> *&father, int i, int order, int m) {
    auto *partition = new leaf_node<K, V>(order, true);
    partition->num_keys = m + ((order % 2) ? 0 : 1);

    partition->next_leaf = next_leaf;
    partition->prev_leaf = this;
    if (next_leaf) {
        next_leaf->prev_leaf = partition;
    }
    next_leaf = partition;

    for (int j = 0; j < m + ((order % 2) ? 0 : 1); ++j) {
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
