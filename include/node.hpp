//
// Created by Juan Diego on 3/7/2023.
//

#ifndef BPLUS_TREE_NODE_HPP
#define BPLUS_TREE_NODE_HPP

#include <functional>
#include <ostream>
#include <string>

template<typename Printable>
using Print = std::function<void(std::ostream &, Printable)>;

/**
 * Node struct in charge of store ordered keys and references to children
 *
 * @tparam K the type of the key stored
 */
template<typename K>
struct node {

    K *keys;                        //< Pointer to the array of keys
    int num_keys;                   //< Number of keys
    node<K> **children;             //< Pointer to the array of pointer to children
    bool is_leaf;                   //< Identifier of leaf node

    /// Creates the node assigning the leaf tag as `false` by default
    explicit node(int order, bool is_leaf = false, int num_keys = 0);

    /// Deletes the node
    virtual ~node();

    /// Deletes all the sub-tree in post-order considering `this` as root
    virtual void kill_self();

    /// Pure virtual member function in charge of split a node into two nodes
    virtual void split(node<K> *&father, int i, int order, int m) = 0;

    /// Reallocates the @param father pointers and keys after the current node is splitted
    void reallocate(node<K> *&father, node<K> *sibling, int i, int m);

    bool has_left_children(int j, node<K> *&left);

    bool has_right_children(int j, node<K> *&right);

    K *max_key();

    K *min_key();


};

template<typename K>
struct internal_node : public node<K> {

    /// Same constructor
    using node<K>::node;

    /// Destructs the internal node
    virtual ~internal_node();

    /// Print all the keys of the node
    void print(std::ostream &os, Print<K> print_key);

    /// Splits a internal node into two nodes an reallocates the father and children references
    void split(node<K> *&father, int i, int order, int m) override;

    std::pair<K, ::node<K> *> pop_back();

    void push_front(node<K>* children);

    void push_back(K key);
};

/**
 * Leaf node struct
 *
 * Keeps a reference of the previous leaf node and the next
 * For any node at the left of a leaf node,
 *     it is true that all their keys are lesser or equal to the keys stored at that leaf node
 *
 * @tparam V the type of the records stored
 */
template<typename K, typename V>
struct leaf_node : public node<K> {

    V *records;                     //< Pointer to array of records
    leaf_node<K, V> *next_leaf;     //< Pointer to the next leaf
    leaf_node<K, V> *prev_leaf;     //< Pointer to the previous leaf

    /// Creates the node assigning the leaf tag as `true` by default
    explicit leaf_node(int order, bool is_leaf = true, int num_keys = 0, leaf_node<K, V> *next_leaf = 0,
                       leaf_node<K, V> *prev_leaf = 0);

    /// Destructs the leaf node
    virtual ~leaf_node();

    /// Print the current node in a key-value format
    void print(std::ostream &os, Print<K> print_key, Print<V> print_value);

    /// Splits a leaf node into two nodes and reallocates the father references
    void split(node<K> *&father, int i, int order, int m) override;

    int locate_key(K key, auto greater);

    void remove_key(K key, int index);

    V pop_back();

    void push_front(V value, auto index);

    void push_back(K key, V value);
};


#endif //BPLUS_TREE_NODE_HPP
