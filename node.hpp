//
// Created by Juan Diego on 3/7/2023.
//

#ifndef BPLUS_TREE_NODE_HPP
#define BPLUS_TREE_NODE_HPP

#include <functional>
#include <ostream>
#include <vector>
#include <string>

template<typename Printable>
using PrintFunction = std::function<void(std::ostream &, Printable)>;

template<typename K>
struct node {
    std::vector<K> keys;
    int num_keys;
    std::vector<node<K> *> children;
    bool is_leaf;

    explicit node(int order, bool is_leaf = false);

    virtual ~node();

    virtual void kill_self();

    virtual void split(node<K> *&father, int i, int order, int m) = 0;
};

template<typename K>
struct internal_node : public node<K> {
    using node<K>::node;

    virtual ~internal_node() = default;

    void print(std::ostream &os, PrintFunction<K> disp_k);

    void split(node<K> *&father, int i, int order, int m) override;
};

template<typename K, typename V>
struct leaf_node : public node<K> {
    std::vector<V> records;
    leaf_node<K, V> *next_leaf;
    leaf_node<K, V> *prev_leaf;

    explicit leaf_node(int order, bool is_leaf = true);

    virtual ~leaf_node();

    void print(std::ostream &os, PrintFunction<K> print_k, PrintFunction<V> print_v);

    void split(node<K> *&father, int i, int order, int m) override;
};


#endif //BPLUS_TREE_NODE_HPP
