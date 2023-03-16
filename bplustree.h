//
// Created by Juan Diego on 3/15/2023.
//

#include "bplustree_def.h"

template<typename KT, typename KV, typename Index>
leafNode<KT, KV> *BPlusTree<KT, KV, Index>::searchNode(Node<KT> *node, KT key) {
    if (!node) {
        return nullptr;
    }
    while (!node->is_leaf) {
        int i = 0;
        for (; i < node->n_keys && node->keys[i] < key; ++i);
        node = node->children[i];
    }
    return reinterpret_cast<leafNode<KT, KV> *>(node);
}

template<typename KT, typename KV, typename Index>
std::list<KV> BPlusTree<KT, KV, Index>::search(KT key) {
    std::list<KV> values;
    leafNode<KT, KV> *leaf = searchNode(root, key);

    while (leaf) {
        for (int i = 0; i < leaf->n_keys; ++i) {
            if (leaf->keys[i] > key)
                return values;
            if (leaf->keys[i] == key)
                values.push_back(leaf->records[i]);
        }
        leaf = leaf->next_leaf;
    }
    return values;
}

template<typename KT, typename KV, typename Index>
std::list<KV> BPlusTree<KT, KV, Index>::searchMin() {
    std::list<KV> values;
    if (this->empty())
        return values;

    Node<KT> *node = root;
    while (!node->is_leaf)
        node = node->children[0];

    auto *leaf = reinterpret_cast<leafNode<KT, KV> *>(node);

    KT min = leaf->keys[0];
    while (leaf) {
        for (int j = 0; j < leaf->n_keys; ++j) {
            if (leaf->keys[j] != min)
                return values;
            values.push_front(leaf->records[j]);
        }
        leaf = leaf->next_leaf;
    }
    return values;
}

template<typename KT, typename KV, typename Index>
std::list<KV> BPlusTree<KT, KV, Index>::searchMax() {
    std::list<KV> values;
    if (this->empty())
        return values;

    Node<KT> *node = root;
    while (!node->is_leaf)
        node = node->children[node->n_keys];

    auto *leaf = reinterpret_cast<leafNode<KT, KV> *>(node);

    KT max = leaf->keys[leaf->n_keys - 1];
    while (leaf) {
        for (int j = (leaf->n_keys - 1); j >= 0; --j) {
            if (leaf->keys[j] != max)
                return values;
            values.push_front(leaf->records[j]);
        }
        leaf = leaf->prev_leaf;
    }
    return values;
}

template<typename KT, typename KV, typename Index>
std::list<KV> BPlusTree<KT, KV, Index>::searchBelow(KT max, bool includeMax) {
    std::list<KV> search;
    leafNode<KT, KV> *leaf = searchNode(root, max);
    auto condition = ([&](KT key) { return includeMax ? (key <= max) : (key < max); });

    while (leaf) {
        for (int i = (leaf->n_keys - 1); i >= 0; --i)
            if (condition(leaf->keys[i]))
                search.push_front(leaf->records[i]);
        leaf = leaf->prev_leaf;
    }
    return search;
}

template<typename KT, typename KV, typename Index>
std::list<KV> BPlusTree<KT, KV, Index>::searchAbove(KT min, bool includeMin) {
    std::list<KV> search;
    leafNode<KT, KV> *leaf = searchNode(root, min);
    auto condition = ([&](KT key) { return includeMin ? (key >= min) : (key > min); });

    while (leaf) {
        for (int i = 0; i < leaf->n_keys; ++i)
            if (condition(leaf->keys[i]))
                search.push_back(leaf->records[i]);
        leaf = leaf->next_leaf;
    }
    return search;
}

template<typename KT, typename KV, typename Index>
std::list<KV> BPlusTree<KT, KV, Index>::searchBetween(KT min, KT max, bool includeMin, bool includeMax) {
    std::list<KV> search;
    leafNode<KT, KV> *leaf = searchNode(root, min);
    auto conditionMax = ([&](KT key) { return includeMax ? (key > max) : (key >= max); });
    auto conditionMin = ([&](KT key) { return includeMin ? (key >= min) : (key > min); });

    while (leaf) {
        for (int i = 0; i < leaf->n_keys; ++i) {
            if (conditionMax(leaf->keys[i]))
                return search;
            if (conditionMin(leaf->keys[i]))
                search.push_back(leaf->records[i]);
        }
        leaf = leaf->next_leaf;
    }
    return search;
}
