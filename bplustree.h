//
// Created by Juan Diego on 3/7/2023.
//

#ifndef BPLUS_TREE_BPLUSTREE_DEF_H
#define BPLUS_TREE_BPLUSTREE_DEF_H

#include <functional>
#include <cmath>
#include <queue>
#include <list>
#include "node.h"

template<typename K, typename V, typename Greater = std::greater<K>, typename Index = std::function<K(V)>>
class b_plus_tree {
private:

    int M;
    int m;

    int n;
    int h;

    Index index;
    Greater greater;
    node<K> *root;

    void nonFullInsert(V value, node<K> *&node) {
        if (node->is_leaf) {
            auto *leaf = reinterpret_cast<leaf_node<K, V> *>(node);
            int i = leaf->n_keys;
            for (; i >= 1 && greater(leaf->keys[i - 1], index(value)); --i) {
                leaf->keys[i] = leaf->keys[i - 1];
                leaf->records[i] = leaf->records[i - 1];
            }

            leaf->keys[i] = index(value);
            leaf->records[i] = value;
            ++leaf->n_keys;
            return;
        }

        ::node<K> *&father = node;
        int j = 0;
        for (; j < father->n_keys && greater(index(value), father->keys[j]); ++j);

        ::node<K> *&child = father->children[j];
        if (child->n_keys < M) {
            nonFullInsert(value, child);
        }
        if (child->n_keys == M) {
            child->split(father, j, M, m);
        }
    }

    leaf_node<K, V> *search_node(node<K> *node, K key);

public:

    explicit b_plus_tree(Index func, int M = 3, Greater greater = Greater()) :
            M(M), n(0), m(int(std::ceil(M / 2.0)) - 1),
            h(-1), root(nullptr), index{func}, greater{greater} {
    }

    ~b_plus_tree() {
        if (!root) {
            return;
        }
        this->root->kill_self();
    }

    void clear() {
        if (!root) {
            return;
        }
        this->root->kill_self();
        this->h = -1;
        this->n = 0;
        this->root = nullptr;
    }

    int height() {
        return this->h;
    }

    int size() {
        return this->n;
    }

    bool empty() {
        return !this->root;
    }

    void insert(V value) {
        if (!root) {
            root = new leaf_node<K, V>(M, true);
            ++this->h;
        }
        if (root->n_keys < M) {
            nonFullInsert(value, root);
        }
        if (root->n_keys == M) {
            node<K> *old_root = this->root;
            root = new internal_node<K>(M);
            root->children[0] = old_root;
            old_root->split(root, 0, M, m);
            ++this->h;
        }
        ++this->n;
    }

    void print(std::ostream &os,
               std::function<void(std::ostream &, V)> print_value = [](std::ostream &os, V value) {
                   os << value;
               },
               std::function<void(std::ostream &, K)> print_key = [](std::ostream &os, K key) {
                   os << key;
               }) {
        if (!root) {
            return;
        }

        int level = 0;
        std::queue<std::pair<node<K> *, int>> Q;

        Q.push({root, level});
        while (!Q.empty()) {
            auto [node, l] = Q.front();
            Q.pop();

            if (l > level) {
                ++level;
                os << std::endl;
            }

            if (node->is_leaf) {
                auto *leaf = reinterpret_cast<leaf_node<K, V> *>(node);
                leaf->print(os, print_key, print_value);
                os << ((leaf->next_leaf) ? " => " : " ");
            } else {
                auto *internalNode = reinterpret_cast<internal_node<K> *>(node);
                internalNode->print(os, print_key);
            }

            if (!node->is_leaf) {
                for (int i = 0; i <= node->n_keys; ++i) {
                    Q.push({node->children[i], l + 1});
                }
            }
        }
    }

    std::list<V> search(K key);

    std::list<V> search_min();

    std::list<V> search_max();

    std::list<V> search_below(K max, bool include_max = false);

    std::list<V> search_above(K min, bool include_min = false);

    std::list<V> search_between(K min, K max, bool include_min = true, bool include_max = true);
};

template<typename K, typename V, typename Index, typename Greater>
leaf_node<K, V> *b_plus_tree<K, V, Index, Greater>::search_node(node<K> *node, K key) {
    if (!node) {
        return nullptr;
    }
    while (!node->is_leaf) {
        int i = 0;
        for (; i < node->n_keys && greater(key, node->keys[i]); ++i);
        node = node->children[i];
    }

    return reinterpret_cast<leaf_node<K, V> *>(node);
}

template<typename K, typename V, typename Index, typename Greater>
std::list<V> b_plus_tree<K, V, Index, Greater>::search(K key) {
    std::list<V> values;
    leaf_node<K, V> *leaf = search_node(root, key);

    while (leaf) {
        for (int i = 0; i < leaf->n_keys; ++i) {
            if (greater(leaf->keys[i], key))
                return values;
            if (!greater(key, leaf->keys[i]))
                values.push_back(leaf->records[i]);
        }
        leaf = leaf->next_leaf;
    }
    return values;
}

template<typename K, typename V, typename Index, typename Greater>
std::list<V> b_plus_tree<K, V, Index, Greater>::search_min() {
    std::list<V> values;
    if (this->empty())
        return values;

    node<K> *node = root;
    while (!node->is_leaf)
        node = node->children[0];

    auto *leaf = reinterpret_cast<leaf_node<K, V> *>(node);

    K min = leaf->keys[0];
    while (leaf) {
        for (int j = 0; j < leaf->n_keys; ++j) {
            if (greater(leaf->keys[j], min))
                return values;
            values.push_front(leaf->records[j]);
        }
        leaf = leaf->next_leaf;
    }
    return values;
}

template<typename K, typename V, typename Index, typename Greater>
std::list<V> b_plus_tree<K, V, Index, Greater>::search_max() {
    std::list<V> values;
    if (this->empty())
        return values;

    node<K> *node = root;
    while (!node->is_leaf)
        node = node->children[node->n_keys];

    auto *leaf = reinterpret_cast<leaf_node<K, V> *>(node);

    K max = leaf->keys[leaf->n_keys - 1];
    while (leaf) {
        for (int j = (leaf->n_keys - 1); j >= 0; --j) {
            if (greater(max, leaf->keys[j]))
                return values;
            values.push_front(leaf->records[j]);
        }
        leaf = leaf->prev_leaf;
    }
    return values;
}

template<typename K, typename V, typename Index, typename Greater>
std::list<V> b_plus_tree<K, V, Index, Greater>::search_below(K max, bool include_max) {
    std::list<V> search;
    leaf_node<K, V> *leaf = search_node(root, max);
    auto include_condition = ([&](K key) { return include_max ? (!greater(key, max)) : (greater(max, key)); });

    while (leaf) {
        for (int i = (leaf->n_keys - 1); i >= 0; --i)
            if (include_condition(leaf->keys[i]))
                search.push_front(leaf->records[i]);
        leaf = leaf->prev_leaf;
    }
    return search;
}

template<typename K, typename V, typename Index, typename Greater>
std::list<V> b_plus_tree<K, V, Index, Greater>::search_above(K min, bool include_min) {
    std::list<V> search;
    leaf_node<K, V> *leaf = search_node(root, min);
    auto include_condition = ([&](K key) { return include_min ? (!greater(min, key)) : (greater(key, min)); });

    while (leaf) {
        for (int i = 0; i < leaf->n_keys; ++i)
            if (include_condition(leaf->keys[i]))
                search.push_back(leaf->records[i]);
        leaf = leaf->next_leaf;
    }
    return search;
}

template<typename K, typename V, typename Index, typename Greater>
std::list<V> b_plus_tree<K, V, Index, Greater>::search_between(K min, K max, bool include_min, bool include_max) {
    std::list<V> search;
    leaf_node<K, V> *leaf = search_node(root, min);
    auto stop_condition = ([&](K key) { return include_max ? (greater(key, max)) : (!greater(max, key)); });
    auto include_condition = ([&](K key) { return include_min ? (!greater(min, key)) : (greater(key, min)); });

    while (leaf) {
        for (int i = 0; i < leaf->n_keys; ++i) {
            if (stop_condition(leaf->keys[i]))
                return search;
            if (include_condition(leaf->keys[i]))
                search.push_back(leaf->records[i]);
        }
        leaf = leaf->next_leaf;
    }
    return search;
}

#endif //BPLUS_TREE_BPLUSTREE_DEF_H
