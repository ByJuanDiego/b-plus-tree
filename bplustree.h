//
// Created by Juan Diego on 3/7/2023.
//

#ifndef BPLUS_TREE_BPLUSTREE_H
#define BPLUS_TREE_BPLUSTREE_H

#include <cmath>
#include <queue>
#include <list>
#include "node.h"

template<typename KT>
class BPlusTree {
private:
    Node<KT> *root;
    int M; // grado u orden del arbol
    int n; // total de elementos en el arbol
    int m; // cantidad minima de keys
    int h; // altura del arbol

    void nonFullInsert(KT key, Node<KT> *&node) {
        if (node->leaf) {
            int i = node->count;
            for (; i >= 1 && key < node->keys[i - 1]; --i) {
                node->keys[i] = node->keys[i - 1];
            }

            node->keys[i] = key;
            ++node->count;
            return;
        }

        Node<KT> *&father = node;
        int j = 0;
        for (; j < father->count && key > father->keys[j]; ++j);

        Node<KT> *&child = father->children[j];
        if (child->count < M) {
            nonFullInsert(key, child);
        }
        if (child->count == M) {
            split(father, j, child);
        }
    }

    void splitInternalNode(Node<KT> *&father, int i, Node<KT> *&child) {
        auto *partition = new Node<KT>(M, false);
        partition->count = m;

        for (int j = 0; j < m; ++j)
            partition->keys[j] = child->keys[j + m + 1 + ((M % 2) ? 0 : 1)];

        for (int j = 0; j < m + 1; ++j)
            partition->children[j] = child->children[j + m + 1 + ((M % 2) ? 0 : 1)];

        for (int j = father->count; j > i; --j)
            father->children[j + 1] = father->children[j];

        for (int j = father->count; j > i; --j)
            father->keys[j] = father->keys[j - 1];

        child->count = m + ((M % 2) ? 0 : 1);

        father->children[i + 1] = partition;
        father->keys[i] = child->keys[m + ((M % 2) ? 0 : 1)];
        father->count++;
    }

    void splitLeaf(Node<KT> *&father, int i, Node<KT> *&leaf) {
        auto *partition = new Node<KT>(M, true);
        partition->count = m + ((M % 2) ? 0 : 1);
        partition->next_leaf = leaf->next_leaf;

        for (int j = 0; j < m + ((M % 2) ? 0 : 1); ++j)
            partition->keys[j] = leaf->keys[j + m + 1];

        for (int j = father->count; j > i; --j)
            father->children[j + 1] = father->children[j];

        for (int j = father->count; j > i; --j)
            father->keys[j] = father->keys[j - 1];

        leaf->next_leaf = partition;
        leaf->count = m + 1;

        father->children[i + 1] = partition;
        father->keys[i] = leaf->keys[m];
        father->count++;
    }

    void split(Node<KT> *&father, int i, Node<KT> *&child) {
        if (child->leaf) {
            splitLeaf(father, i, child);
        } else {
            splitInternalNode(father, i, child);
        }
    }

public:

    explicit BPlusTree(int M) : M(M), n(0), m(static_cast<int>(std::ceil(M / 2.0)) - 1), h(-1), root(nullptr) {}

    void insert(KT key) {
        if (!root) {
            root = new Node<KT>(M, true);
        }
        if (root->count < M) {
            nonFullInsert(key, root);
        }
        if (root->count == M) {
            Node<KT> *old_root = this->root;
            root = new Node<KT>(M);
            root->children[0] = old_root;
            split(root, 0, old_root);
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
            os << (node->leaf ? "->" : step);

            if (!node->leaf) {
                for (int i = 0; i <= node->count; ++i) {
                    Q.push({node->children[i], l + 1});
                }
            }
        }
    }

    std::list<KT> rangeSearch(KT min, KT max) {
        std::list<KT> search;

        Node<KT> *node = root;
        while (!node->leaf) {
            int i = 0;
            for (; i < node->count && node->keys[i] < min; ++i);
            node = node->children[i];
        }

        while (node) {
            for (int i = 0; i < node->count; ++i) {
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
