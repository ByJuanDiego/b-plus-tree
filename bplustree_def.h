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

    // operations are made on a O(log_{M}(n) + k) time complexity

    std::list<KV> search(KT key);
    // time complexity gets worse when having more registers with the same key value

    std::list<KV> searchMin();
    // time complexity gets worse when having many registers with the minimum key value

    std::list<KV> searchMax();
    // time complexity gets worse when having many registers with the maximum key value

    std::list<KV> searchBelow(KT max, bool includeMax = true);
    // time complexity gets worse when the limit max value gets closer to the maximum value in the tree

    std::list<KV> searchAbove(KT min, bool includeMin = true);
    // time complexity gets worse when the limit min value gets closer to the minimum value in the tree

    std::list<KV> searchBetween(KT min, KT max, bool includeMin = true, bool includeMax = true);
    // time complexity gets worse when the difference between max and min is greater
};

#endif //BPLUS_TREE_BPLUSTREE_DEF_H
