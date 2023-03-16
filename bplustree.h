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

template<typename KT, typename KV, typename Index = std::function<KT(KV)>>
class BPlusTree {
private:

    int M;
    int m;

    int n;
    int h;

    Index f;
    Node<KT> *root;

    void nonFullInsert(KV value, Node<KT> *&node) {
        if (node->is_leaf) {
            auto *leaf = reinterpret_cast<leafNode<KT, KV> *>(node);
            int i = leaf->n_keys;
            for (; i >= 1 && f(value) < leaf->keys[i - 1]; --i) {
                leaf->keys[i] = leaf->keys[i - 1];
                leaf->records[i] = leaf->records[i - 1];
            }

            leaf->keys[i] = f(value);
            leaf->records[i] = value;
            ++leaf->n_keys;
            return;
        }

        Node<KT> *&father = node;
        int j = 0;
        for (; j < father->n_keys && f(value) > father->keys[j]; ++j);

        Node<KT> *&child = father->children[j];
        if (child->n_keys < M) {
            nonFullInsert(value, child);
        }
        if (child->n_keys == M) {
            child->split(father, j, M, m);
        }
    }

    leafNode<KT, KV> *searchNode(Node<KT> *node, KT key);

public:

    explicit BPlusTree(Index func, int M = 3) :
            M(M), n(0), m(int(std::ceil(M / 2.0)) - 1),
            h(-1), root(nullptr), f{func} {
    }

    ~BPlusTree() {
        if (!root) {
            return;
        }
        this->root->killSelf();
    }

    void clear() {
        if (!root) {
            return;
        }
        this->root->killSelf();
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

    void insert(KV value) {
        if (!root) {
            root = new leafNode<KT, KV>(M, true);
            ++this->h;
        }
        if (root->n_keys < M) {
            nonFullInsert(value, root);
        }
        if (root->n_keys == M) {
            Node<KT> *old_root = this->root;
            root = new internalNode<KT>(M);
            root->children[0] = old_root;
            old_root->split(root, 0, M, m);
            ++this->h;
        }
        ++this->n;
    }

    void print(std::ostream &os, const std::string &step = " ") const {
        if (!root) {
            return;
        }

        int level = 0;
        std::queue<std::pair<Node<KT> *, int>> Q;

        Q.push({root, level});
        while (!Q.empty()) {
            auto [node, l] = Q.front();
            Q.pop();

            if (l > level) {
                ++level;
                os << std::endl;
            }

            node->print(os, M);
            os << (node->is_leaf && reinterpret_cast<leafNode<KT, KV> *>(node)->next_leaf ? "=>" : step);

            if (!node->is_leaf) {
                for (int i = 0; i <= node->n_keys; ++i) {
                    Q.push({node->children[i], l + 1});
                }
            }
        }
    }

    std::list<KV> search(KT key);

    std::list<KV> searchMin();

    std::list<KV> searchMax();

    std::list<KV> searchBelow(KT max, bool includeMax = true);

    std::list<KV> searchAbove(KT min, bool includeMin = true);

    std::list<KV> searchBetween(KT min, KT max, bool includeMin = true, bool includeMax = true);
};


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

#endif //BPLUS_TREE_BPLUSTREE_DEF_H
