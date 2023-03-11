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

template<typename KT, typename KV>
class BPlusTree {
private:
    Node<KT> *root;
    int M; // grado u orden del arbol
    int n; // total de elementos en el arbol
    int m; // cantidad minima de keys
    int h; // altura del arbol

    void nonFullInsert(KT key, Node<KT> *&node) {
        if (node->is_leaf) {
            int i = node->n_children;
            for (; i >= 1 && key < node->keys[i - 1]; --i) {
                node->keys[i] = node->keys[i - 1];
            }

            node->keys[i] = key;
            ++node->n_children;
            return;
        }

        Node<KT> *&father = node;
        int j = 0;
        for (; j < father->n_children && key > father->keys[j]; ++j);

        Node<KT> *&child = father->children[j];
        if (child->n_children < M) {
            nonFullInsert(key, child);
        }
        if (child->n_children == M) {
            child->split(father, j, M, m);
        }
    }

public:

    explicit BPlusTree(int M = 3) : M(M), n(0), m(static_cast<int>(std::ceil(M / 2.0)) - 1), h(-1), root(nullptr) {}

    ~BPlusTree() {
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

    void insert(KT key) {
        if (!root) {
            root = new leafNode<KT, KV>(M, true);
        }
        if (root->n_children < M) {
            nonFullInsert(key, root);
        }
        if (root->n_children == M) {
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
            os << (node->is_leaf ? "->" : step);

            if (!node->is_leaf) {
                for (int i = 0; i <= node->n_children; ++i) {
                    Q.push({node->children[i], l + 1});
                }
            }
        }
    }

    std::list<KT> rangeSearch(KT min, KT max) {
        std::list<KT> search;

        Node<KT> *node = root;
        while (!node->is_leaf) {
            int i = 0;
            for (; i < node->n_children && node->keys[i] < min; ++i);
            node = node->children[i];
        }

        while (node) {
            for (int i = 0; i < node->n_children; ++i) {
                if (node->keys[i] > max) {
                    return search;
                }
                if (node->keys[i] >= min) {
                    search.push_back(node->keys[i]);
                }
            }
            node = node->next_leaf;
        }
        return search;
    }
};

#endif //BPLUS_TREE_BPLUSTREE_H
