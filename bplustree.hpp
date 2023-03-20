//
// Created by Juan Diego on 3/7/2023.
//

#ifndef BPLUS_TREE_BPLUSTREE_DEF_H
#define BPLUS_TREE_BPLUSTREE_DEF_H

#include <cmath>
#include <queue>
#include <list>

#include "node.cpp"

template<typename Printable>
PrintFunction<Printable> display = [](std::ostream &os, const Printable &p) { os << p; };

template<int M, typename K, typename V, typename Greater = std::greater<K>, typename Index = std::function<K(V)>>
class b_plus_tree {
private:

    int m;
    int n;
    int h;
    node<K> *root;
    Index index;
    Greater greater;

    void non_full_insert(V value, node<K> *&node);

    leaf_node<K, V> *search_node(node<K> *node, K key);

public:

    explicit b_plus_tree(Index index, Greater greater = Greater());

    ~b_plus_tree();

    int size();

    bool empty();

    void clear();

    int height();

    void insert(V value);

    std::list<V> search(K key);

    std::list<V> search_min();

    std::list<V> search_max();

    std::list<V> search_below(K max, bool include_max = false);

    std::list<V> search_above(K min, bool include_min = false);

    std::list<V> search_between(K min, K max, bool include_min = true, bool include_max = true);

    void print(std::ostream &os, PrintFunction<V> disp_v = display<V>, PrintFunction<K> disp_k = display<K>);
};


#endif //BPLUS_TREE_BPLUSTREE_DEF_H
