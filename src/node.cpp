//
// Created by juandiego on 3/19/23.
//

#include "node.hpp"

//-----------------------------------------------------------------------------

template<typename K>
node<K>::node(int order, bool is_leaf, int num_keys) : num_keys(num_keys), is_leaf(is_leaf) {
    keys = new K[order]{K()};
    children = new node<K> *[order + 1]{nullptr};
}

//-----------------------------------------------------------------------------

template<typename K>
node<K>::~node() {
    delete[] keys;
    delete[] children;
}

//-----------------------------------------------------------------------------

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

//-----------------------------------------------------------------------------

template<typename K>
void node<K>::reallocate(node<K> *&father, node<K> *sibling, int i, int m) {
    for (int j = father->num_keys; j > i; --j)
        father->keys[j] = father->keys[j - 1];

    for (int j = father->num_keys; j > i; --j)
        father->children[j + 1] = father->children[j];

    father->children[i + 1] = sibling;
    father->keys[i] = this->keys[m];
    father->num_keys++;
}

//-----------------------------------------------------------------------------

template<typename K>
void internal_node<K>::split(node<K> *&father, int i, int order, int m) {
    auto *sibling = new internal_node<K>(order, false, (order / 2));

    for (int j = 0; j < sibling->num_keys; ++j)
        sibling->keys[j] = this->keys[j + m + 1];

    for (int j = 0; j < (sibling->num_keys + 1); ++j)
        sibling->children[j] = this->children[j + m + 1];

    this->num_keys = m;
    this->reallocate(father, sibling, i, m);
}

//-----------------------------------------------------------------------------

template<typename K>
internal_node<K>::~internal_node() = default;

//-----------------------------------------------------------------------------

template<typename K>
void internal_node<K>::print(std::ostream &os, Print<K> print_key) {
    os << "[";
    for (int i = 0; i < this->num_keys; ++i) {
        print_key(os, this->keys[i]);
        os << ((i < (this->num_keys - 1)) ? "|" : "");
    }
    os << "]";
}

//-----------------------------------------------------------------------------

template<typename K, typename V>
leaf_node<K, V>::leaf_node(int order, bool is_leaf, int num_keys, leaf_node<K, V> *next_leaf, leaf_node<K, V> *prev_leaf) :
        node<K>(order, is_leaf, num_keys),
        next_leaf(next_leaf),
        prev_leaf(prev_leaf) {
    records = new V[order];
}

//-----------------------------------------------------------------------------

template<typename K, typename V>
leaf_node<K, V>::~leaf_node() {
    delete[] records;
}

//-----------------------------------------------------------------------------

template<typename K, typename V>
void leaf_node<K, V>::split(node<K> *&father, int i, int order, int m) {
    auto *sibling = new leaf_node<K, V>(order, true, (order / 2), this->next_leaf, this);

    if (next_leaf)
        next_leaf->prev_leaf = sibling;
    next_leaf = sibling;

    for (int j = 0; j < sibling->num_keys; ++j) {
        sibling->keys[j] = this->keys[j + m + 1];
        sibling->records[j] = this->records[j + m + 1];
    }

    this->num_keys = m + 1;
    this->reallocate(father, sibling, i, m);
}

//-----------------------------------------------------------------------------

template<typename K, typename V>
void leaf_node<K, V>::print(std::ostream &os, Print<K> print_key, Print<V> print_value) {
    os << "[";
    for (int i = 0; i < this->num_keys; ++i) {
        os << "(";
        print_key(os, this->keys[i]);
        os << ":";
        print_value(os, this->records[i]);
        os << ")";
        os << ((i < (this->num_keys - 1)) ? "|" : "");
    }
    os << "]";
}

//-----------------------------------------------------------------------------
