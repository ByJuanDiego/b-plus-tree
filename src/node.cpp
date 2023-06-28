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
bool node<K>::has_left_children(int j, node<K> *&left) {
    bool flag = j > 0;
    if (flag) {
        left = this->children[j - 1];
    }
    return flag;
}

//-----------------------------------------------------------------------------

template<typename K>
bool node<K>::has_right_children(int j, node<K> *&right) {
    bool flag = j < this->num_keys;
    if (flag) {
        right = this->children[j + 1];
    }
    return flag;
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
leaf_node<K, V>::leaf_node(int order, bool is_leaf, int num_keys, leaf_node<K, V> *next_leaf,
                           leaf_node<K, V> *prev_leaf) :
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

template<typename K, typename V>
int leaf_node<K, V>::locate_key(K key, auto greater) {
    int index = 0;
    for (; (index < this->num_keys) && greater(key, this->keys[index]); ++index);

    if (index == this->num_keys || this->keys[index] != key) {
        return -1;
    }
    return index;
}

//-----------------------------------------------------------------------------

template<typename K, typename V>
void leaf_node<K, V>::remove_key(K key, int index) {
    for (int i = index; i < (this->num_keys - 1); ++i) {
        this->keys[i] = this->keys[i + 1];
        this->records[i] = this->records[i + 1];
    }
    --this->num_keys;
}

//-----------------------------------------------------------------------------

template<typename K>
K *node<K>::max_key() {
    return (this->num_keys > 0) ? &this->keys[this->num_keys - 1] : nullptr;
}

//-----------------------------------------------------------------------------

template<typename K>
K *node<K>::min_key() {
    return (this->num_keys > 0) ? &this->keys[0] : nullptr;;
}

//-----------------------------------------------------------------------------

template<typename K, typename V>
void leaf_node<K, V>::push_front(V value, auto index) {
    for (int i = (this->num_keys - 1); i > 0; --i) {
        this->keys[i] = this->keys[i - 1];
        this->records[i] = this->records[i - 1];
    }

    this->records[0] = value;
    this->keys[0] = index(value);
}

//-----------------------------------------------------------------------------

template<typename K, typename V>
V leaf_node<K, V>::pop_back() {
    return this->records[--this->num_keys];
}

//-----------------------------------------------------------------------------

template<typename K>
std::pair<K, ::node<K> *> internal_node<K>::pop_back() {
    ::node<K> *child = this->children[this->num_keys];

    node<K> *node = child;
    while (!node->is_leaf)
        node = node->children[node->num_keys];
    K key = node->keys[node->num_keys - 1];

    this->num_keys--;
    return {key, child};
}

//-----------------------------------------------------------------------------

template<typename K>
void internal_node<K>::push_front(K key, node<K> *children) {
    for (int i = this->num_keys; i > 0; --i) {
        this->keys[i] = this->keys[i - 1];
    }

    for (int i = (this->num_keys + 1); i > 0; --i) {
        this->children[i] = this->children[i - 1];
    }

    this->keys[0] = key;
    this->children[0] = children;
}
