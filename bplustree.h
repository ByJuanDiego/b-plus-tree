//
// Created by Juan Diego on 3/7/2023.
//

#ifndef BPLUS_TREE_BPLUSTREE_H
#define BPLUS_TREE_BPLUSTREE_H

#include <functional>
#include <cmath>
#include <queue>
#include <list>
#include "node.h"

template<typename KT, typename KV, typename Index = std::function<KT(KV)>>
class BPlusTree {
private:

    Index f;
    Node<KT> *root;

    int M; // degree of the tree
    int m; // minimum number of keys in a node

    int n; // total of elements in leaf nodes
    int h; // height of the tree

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

    leafNode<KT, KV> *searchNode(Node<KT> *node, KT key) {
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

public:

    explicit BPlusTree(Index func, int M = 3) :
            M(M), n(0), m(int(std::ceil(M / 2.0)) - 1),
            h(-1), root(nullptr), f{func} {}

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

    std::list<KV> searchEqual(KT key) {
        std::list<KV> search;
        leafNode<KT, KV> *leaf = searchNode(root, key);

        while (leaf) {
            for (int i = 0; i < leaf->n_keys; ++i) {
                if (leaf->keys[i] > key)
                    return search;
                if (leaf->keys[i] == key)
                    search.push_back(leaf->records[i]);
            }
            leaf = leaf->next_leaf;
        }
        return search;
    }

    std::list<KV> searchBetween(KT min, KT max) {
        std::list<KV> search;
        leafNode<KT, KV> *leaf = searchNode(root, min);

        while (leaf) {
            for (int i = 0; i < leaf->n_keys; ++i) {
                if (leaf->keys[i] > max)
                    return search;
                if (leaf->keys[i] >= min)
                    search.push_back(leaf->records[i]);
            }
            leaf = leaf->next_leaf;
        }
        return search;
    }

    std::list<KV> searchAbove(KT min) {
        std::list<KV> search;
        leafNode<KT, KV> *leaf = searchNode(root, min);

        while (leaf) {
            for (int i = 0; i < leaf->n_keys; ++i)
                if (leaf->keys[i] > min)
                    search.push_back(leaf->records[i]);
            leaf = leaf->next_leaf;
        }
        return search;
    }

    std::list<KV> searchBelow(KT max) {
        std::list<KV> search;
        leafNode<KT, KV> *leaf = searchNode(root, max);

        while (leaf) {
            for (int i = (leaf->n_keys - 1); i >= 0; --i)
                if (leaf->keys[i] < max)
                    search.push_front(leaf->records[i]);
            leaf = leaf->prev_leaf;
        }
        return search;
    }

};

#endif //BPLUS_TREE_BPLUSTREE_H
